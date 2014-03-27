//
//  VFFileManager.m
//
//  Created by Dima Bart on 2014-03-12.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import <sys/xattr.h>

#import "VFFileManager.h"
#import "VFTokenCollection.h"

#pragma mark - Private -
static VFFileType VFFileInfoGetType(VFFileInfo info) {
    mode_t mode = info->mode;
    if (S_ISBLK(mode) != 0) {
        return VFFileTypeBlockSpecial;
        
    } else if (S_ISCHR(mode) != 0) {
        return VFFileTypeCharSpecial;
        
    } else if (S_ISDIR(mode) != 0) {
        return VFFileTypeDirectory;
        
    } else if (S_ISFIFO(mode) != 0) {
        return VFFileTypeFIFO;
        
    } else if (S_ISREG(mode) != 0) {
        return VFFileTypeFile;
        
    } else if (S_ISLNK(mode) != 0) {
        return VFFileTypeSymLink;
        
    } else if (S_ISSOCK(mode) != 0) {
        return VFFileTypeSocket;
        
    } else if (S_ISWHT(mode) != 0) {
        return VFFileTypeWhiteout;
        
    } else {
        return VFFileTypeUndefined;
    }
    
    
}

static char * VFGetPermissions(uint16_t mode) {
    char *permissions = NULL;
    asprintf(&permissions, "%o", (mode & ALLPERMS));
    return permissions;
}

static char * VFCreateDirectoryPath(const char *path) {
    char *clean_path    = strdup(path);
    size_t path_length  = strlen(path);
    
    if (path[path_length - 1] != '/') {
        if (clean_path) {
            free(clean_path);
        }
        clean_path = VFJoin(path, "/");
    }
    return clean_path;
}

static struct stat VFFileStat(const char *path, char **error) {
    struct stat file;
    int status = stat(path, &file);
    if (status == -1 && error) {
        *error = strerror(errno);
    }
    return file;
}

static struct statvfs VFSystemStat(const char *path, char **error) {
    struct statvfs system;
    int status = statvfs(path, &system);
    if (status == -1 && error) {
        *error = strerror(errno);
    }
    return system;
}

static BOOL VFFileCopy(const char *from, const char *to, char **error) {
    
    BOOL success = NO;
    
    // Pre-flight check
    if (!from || !to || strcmp(from, to) == 0) {
        if (error) {
            *error = "Invalid origin or destination path";
        }
        return success;
    }
    
    // Check file type
    struct stat from_stat = VFFileStat(from, NULL);
    if (S_ISDIR(from_stat.st_mode)) {
        if (error) {
            *error = "Could not copy, source file is a directory";
        }
        return success;
    }
    
    // Optimal buffer size for copy
    size_t block_size = from_stat.st_blksize;
    if (block_size < 1) {
        block_size = 4096;
    }
    
    // Open source file
    int from_file = open(from, O_RDONLY);
    if (from_file != -1) {
        
        // Open destination for write (create and truncate)
        int to_file = open(to, O_WRONLY | O_CREAT | O_TRUNC, from_stat.st_mode);
        if (to_file != -1) {
            
            int error_occured     = 0;
            ssize_t bytes_read    = 0;
            ssize_t bytes_written = 0;
            
            uint8_t buffer[block_size];
            
            while ((bytes_read = read(from_file, buffer, block_size)) > 0) {
                if ((bytes_written = write(to_file, buffer, bytes_read)) != bytes_read) {
                    error_occured = 1;
                    break;
                }
            }
            
            // Extended attribute support
          #ifdef _SYS_XATTR_H_
            ssize_t size = fgetxattr(from_file, XATTR_FINDERINFO_NAME, NULL, 0, 0, 0);
            void *value  = malloc(size);
            fgetxattr(from_file, XATTR_FINDERINFO_NAME, value, size, 0, 0);
            if (size != -1) {
                fsetxattr(to_file, XATTR_FINDERINFO_NAME, value, size, 0, 0);
            }
          #endif
            
            close(to_file);
            
            // Clean-up created file on error
            if (error_occured) {
                if (error) {
                    *error = "Could not write buffer to output file";
                }
                unlink(to);
            }
            
            success = (error_occured == 0);
            
        } else {
            if (error) {
                *error = "Could not open destination file";
            }
        }
        
        close(from_file);
        
    } else {
        if (error) {
            *error = "Could not open source file";
        }
    }
    
    return success;
}

static BOOL VFIsFile(const char *name) {
    if (name) {
        return (strcmp(name, ".") != 0 && strcmp(name, "..") != 0);
    }
    return NO;
}

static BOOL VFIsHidden(const char *name) {
    if (name) {
        return (name[0] == '.');
    }
    return NO;
}

#pragma mark - Path Operations -
char * VFPathCopyLastComponent(const char *path) {
    char *reference;
    char *stack      = strdup(path);
    char *token      = strtok_r(stack, "/", &reference);
    char *last_token = NULL;
    while (token) {
        last_token = token;
        token      = strtok_r(NULL, "/", &reference);
    }
    free(stack);
    
    return strdup(last_token);
}

#pragma mark - VFFileInfo -
VFFileInfo VFFileInfoCreate(const char *path, char **error) {
    if (!path) {
        if (error) {
            *error = "Invalid path specified";
        }
        return NO;
    }
    
    struct stat file = VFFileStat(path, error);
    VFFileInfo info  = malloc(sizeof(_VFFileInfo));
    if (info) {
        info->time_accessed       = file.st_atimespec.tv_sec;
        info->time_modified       = file.st_mtimespec.tv_sec;
        info->time_status_changed = file.st_ctimespec.tv_sec;
        info->mode                = file.st_mode;
        info->user_id             = file.st_uid;
        info->group_id            = file.st_gid;
        info->file_serial         = file.st_ino;
        info->device_id           = file.st_dev;
        info->size                = file.st_size;
        info->path                = strdup(path);
        info->type                = VFFileInfoGetType(info);
        info->permissions         = VFGetPermissions(file.st_mode);
        
        return info;
    }
    return NULL;
}

VFFileInfo VFFileInfoCopy(VFFileInfo info) {
    if (info) {
        VFFileInfo new_info = malloc(sizeof(_VFFileInfo));
        if (new_info) {
            new_info->time_accessed       = info->time_accessed;
            new_info->time_modified       = info->time_modified;
            new_info->time_status_changed = info->time_status_changed;
            new_info->mode                = info->mode;
            new_info->user_id             = info->user_id;
            new_info->group_id            = info->group_id;
            new_info->file_serial         = info->file_serial;
            new_info->device_id           = info->device_id;
            new_info->size                = info->size;
            new_info->path                = strdup(info->path);
            new_info->type                = info->type;
            new_info->permissions         = strdup(info->permissions);

            return new_info;
        }
    }
    return NULL;
}

VFFileUser VFFileInfoCopyUser(VFFileInfo info, char **error) {
    if (info) {
        struct passwd *sys_user = getpwuid(info->user_id);
        if (sys_user) {
            
            VFFileUser user = malloc(sizeof(_VFFileUser));
            if (user) {
                user->user_id   = sys_user->pw_uid;
                user->user_name = strdup(sys_user->pw_name);
                
                return user;
            }
        }
        
        if (error) *error = strerror(errno);
    }
    return NULL;
}

VFFileGroup VFFileInfoCopyGroup(VFFileInfo info, char **error) {
    if (info) {
        struct group *sys_group = getgrgid(info->group_id);
        if (sys_group) {
            
            VFFileGroup group = malloc(sizeof(_VFFileGroup));
            if (group) {
                group->group_id   = sys_group->gr_gid;
                group->group_name = strdup(sys_group->gr_name);
                
                return group;
            }
        }
        
        if (error) *error = strerror(errno);
    }
    return NULL;
}

void VFFileUserRelease(VFFileUser user) {
    if (user) {
        free(user->user_name);
        free(user);
    }
}

void VFFileGroupRelease(VFFileGroup group) {
    if (group) {
        free(group->group_name);
        free(group);
    }
}

void VFFileInfoRelease(VFFileInfo info) {
    if (info) {
        free(info->permissions);
        free(info->path);
        free(info);
    }
}

#pragma mark - VFSystemInfo -
VFSystemInfo VFSystemInfoCreate(const char *path, char **error) {
    if (!path) {
        if (error) {
            *error = "Invalid path specified";
        }
        return NO;
    }
    
    struct statvfs system_stat = VFSystemStat(path, error);
    VFSystemInfo info          = malloc(sizeof(_VFSystemInfo));
    if (info) {
        info->system_id        = system_stat.f_fsid;
        info->max_file_length  = system_stat.f_namemax;
        info->block_size       = system_stat.f_frsize;
        info->blocks_count     = system_stat.f_blocks;
        info->blocks_available = system_stat.f_bavail;
        info->blocks_free      = system_stat.f_bfree;
        info->files_count      = system_stat.f_files;
        info->files_available  = system_stat.f_favail;
        info->files_free       = system_stat.f_ffree;
        
        return info;
    }
    return NULL;
}

void VFSystemInfoRelease(VFSystemInfo info) {
    if (info) {
        free(info);
    }
}

#pragma mark - VFSystemDescription -
VFDiskDescription VFDiskDescriptionCreate(const char *path, char **error) {
    if (!path) {
        if (error) {
            *error = "Invalid path specified";
        }
        return NO;
    }
    
    struct statvfs system_stat = VFSystemStat(path, error);
    VFDiskDescription info     = malloc(sizeof(_VFDiskDescription));
    if (info) {
        info->system_id        = system_stat.f_fsid;
        info->max_file_length  = system_stat.f_namemax;
        info->bytes_total      = (uint64_t)system_stat.f_blocks * (uint64_t)system_stat.f_frsize;
        info->bytes_available  = (uint64_t)system_stat.f_bavail * (uint64_t)system_stat.f_frsize;
        info->bytes_free       = (uint64_t)system_stat.f_bfree  * (uint64_t)system_stat.f_frsize;
        
        return info;
    }
    return NULL;
}

void VFSystemDescriptionRelease(VFSystemInfo info) {
    
}

#pragma mark - File Operations -
BOOL VFFileExists(const char *path) {
    if (!path) {
        return NO;
    }
    
    return (access(path, F_OK) == 0);
}

BOOL VFFileIsDirectory(const char *path, char **error) {
    if (!path) {
        if (error) {
            *error = "Invalid path or permissions specified";
        }
        return NO;
    }
    
    struct stat info = VFFileStat(path, error);
    return S_ISDIR(info.st_mode);
}

BOOL VFCreateDirectoryPermissions(const char *path, mode_t permissions, char **error) {
    if (!path || !permissions) {
        if (error) {
            *error = "Invalid path or permissions specified";
        }
        return NO;
    }
    
    int status = mkdir(path, permissions);
    if (status == -1 && error) {
        *error = strerror(errno);
    }
    return (status == 0);
}

BOOL VFCreateDirectory(const char *path, char **error) {
    return VFCreateDirectoryPermissions(path, 0755, error);
}

BOOL VFFileDelete(const char *path, char **error) {
    if (!path) {
        if (error) {
            *error = "Invalid path specified";
        }
        return NO;
    }
    
    int status;
    if (VFFileIsDirectory(path, error)) {
        status = rmdir(path);
    } else {
        status = unlink(path);
    }
    
    if (status == -1 && error) {
        *error = strerror(errno);
    }
    return (status == 0);
}

BOOL VFMoveFile(const char *from, const char *to, char **error) {
    if (!from || !to || strcmp(from, to) == 0) {
        if (error) {
            *error = "Invalid origin or destination path";
        }
        return NO;
    }
    
    int status = rename(from, to);
    if (status == -1 && error) {
        *error = strerror(errno);
    }
    return (status == 0);
}

BOOL VFCopyFile(const char *from, const char *to, char **error) {
    
    BOOL success = NO;
    
    // Pre-flight check
    if (!from || !to || strcmp(from, to) == 0) {
        if (error) {
            *error = "Invalid origin or destination path";
        }
        return success;
    }
    
    // Check file type
    struct stat from_stat = VFFileStat(from, NULL);
    if (S_ISDIR(from_stat.st_mode)) {
        
        if (VFCreateDirectory(to, error)) {
            __block int errors = 0;
            VFEnumerateDirectory(from, VFFileEnumerationOptionHidden, ^(void *info, char *error) {
                if (info) {
                    
                    char *file_name   = VFPathCopyLastComponent(info);
                    char *destination = VFCreateDirectoryPath(to);
                    char *new_path    = VFJoin(destination, file_name);
                    if (!VFCopyFile(info, new_path, NULL)) {
                        errors++;
                        return;
                    }
                    
                    free(file_name);
                    free(destination);
                    free(new_path);
                }
            });
            
            success = (errors == 0);
        }
        
    } else {
        success = VFFileCopy(from, to, error);
    }

    return success;
}

#pragma mark - File Scanning -
void VFEnumerateFileBuffer(const char *path, VFFileBytesEnumerationBlock block) {
    
    // Check file type
    struct stat file_stat = VFFileStat(path, NULL);
    if (S_ISDIR(file_stat.st_mode)) {
        block(NULL, -1, "Failed to enumerate file bytes. File is a directory");
        return;
    }
    
    // Setup buffer
    size_t block_size = file_stat.st_blksize;
    if (block_size < 1) {
        block_size = 4096;
    }
    
    int from_file = open(path, O_RDONLY);
    if (from_file != -1) {
        
        uint8_t buffer[block_size];
        ssize_t bytes_read = 0;
        
        while ((bytes_read = read(from_file, buffer, block_size)) > 0) {
            
            if (bytes_read > 0) {
                uint8_t clean_buffer[bytes_read];
                memcpy(clean_buffer, buffer, bytes_read);
                block(buffer, bytes_read, NULL);
            }
        }
        
    } else {
        block(NULL, -1, strerror(errno));
    }
}

#pragma mark - Directory Enumeration -
void VFEnumerateDirectory(const char *path, VFFileEnumerationOption options, VFDirectoryEnumerationBlock block) {
    if (!path || !block) {
        return;
    }
    
    char *clean_path = VFCreateDirectoryPath(path);

    DIR *directory = opendir(path);
    if (directory) {
        
        BOOL is_deep   = (options & VFFileEnumerationOptionDeep);
        BOOL is_detail = (options & VFFileEnumerationOptionDetail);
        BOOL is_hidden = (options & VFFileEnumerationOptionHidden);
        
        for (struct dirent *entry = NULL; (entry = readdir(directory)) != NULL;) {
            
            if (VFIsFile(entry->d_name)) {
                if (!VFIsHidden(entry->d_name) || is_hidden) {
                    char *file_path = VFJoin(clean_path, entry->d_name);
                    
                    // File path option (default)
                    if (!is_detail) {
                        
                        block(file_path, NULL);
                        
                        // Deep option set
                        if (is_deep && S_ISDIR(DTTOIF(entry->d_type))) {
                            VFEnumerateDirectory(file_path, options, block);
                        }
                        
                    // VFFileInfo struct (detail option)
                    } else {
                        
                        char *error     = NULL;
                        VFFileInfo info = VFFileInfoCreate(file_path, &error);
                        block(info, error);
                        
                        // Deep option set
                        if (is_deep) {
                            if (info->type == VFFileTypeDirectory) {
                                VFEnumerateDirectory(file_path, options, block);
                            }
                        }
                        
                        VFFileInfoRelease(info);
                        
                    }
                    
                    free(file_path);
                }
            }
            
        }
        
        closedir(directory);
    } else {
        block(NULL, strerror(errno));
    }
    
    free(clean_path);
}











