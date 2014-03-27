//
//  VFFileManager.h
//  DictionaryParser
//
//  Created by Dima Bart on 2014-03-12.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import <stdlib.h>
#import <stdio.h>
#import <string.h>
#import <unistd.h>
#import <fcntl.h>
#import <errno.h>
#import <pwd.h>
#import <grp.h>
#import <sys/stat.h>
#import <sys/statvfs.h>
#import <sys/dirent.h>
#import <stdbool.h>




#ifndef OBJC_BOOL_DEFINED
    typedef signed char BOOL;
    #define YES (BOOL)1
    #define NO  (BOOL)0
#endif




// MARK: - Type Definitions - Blocks -
typedef void (^VFDirectoryEnumerationBlock)(void *info, char *error);
typedef void (^VFFileBytesEnumerationBlock)(uint8_t *bytes, ssize_t bytes_read, char *error);

// MARK: - Type Definitions - Enums -
typedef enum {
    VFFileTypeUndefined    = 0,
    VFFileTypeBlockSpecial = 1,
    VFFileTypeCharSpecial  = 2,
    VFFileTypeDirectory    = 3,
    VFFileTypeFIFO         = 4,
    VFFileTypeFile         = 5,
    VFFileTypeSymLink      = 6,
    VFFileTypeSocket       = 7,
    VFFileTypeWhiteout     = 8,
} VFFileType;

typedef enum {
    VFFileEnumerationOptionNone   = 0,
    VFFileEnumerationOptionDeep   = 1 << 0,
    VFFileEnumerationOptionHidden = 1 << 1,
    VFFileEnumerationOptionDetail = 1 << 2,
} VFFileEnumerationOption;

/*
 * =============================
 *       Path Operations
 * =============================
 *
 */
// MARK: - Path Operations -
char * VFPathCopyLastComponent(const char *path);


/*
 * =============================
 *     VFFileInfo & Related
 * =============================
 *
 */
// MARK: - VFFileInfo -

/*
 * Indicated by + are the corresponding NSFileManager file attributes
 * covered by VFFileInfo
 *
 * +    NSFileCreationDate          = "2014-01-27 18:12:37 +0000";
 *    NSFileExtensionHidden         = 0;
 * +    NSFileGroupOwnerAccountID   = 20;
 * +    NSFileGroupOwnerAccountName = staff;
 *    NSFileHFSCreatorCode          = 0;
 *    NSFileHFSTypeCode             = 0;
 * +    NSFileModificationDate      = "2014-01-27 18:12:41 +0000";
 * +    NSFileOwnerAccountID        = 501;
 * +    NSFileOwnerAccountName      = dbart;
 * +    NSFilePosixPermissions      = 420;
 *    NSFileReferenceCount          = 1;
 * +    NSFileSize                  = 253625;
 * +    NSFileSystemFileNumber      = 21121935;
 * +    NSFileSystemNumber          = 16777218;
 * +    NSFileType                  = NSFileTypeRegular;
 *
 */

typedef struct __VFFileInfo {
    long       time_accessed;
    long       time_modified;
    long       time_status_changed;
    uint16_t   mode;
    uint64_t   file_serial;
    uint32_t   user_id;
    uint32_t   group_id;
    int32_t    device_id;
    int64_t    size;
    VFFileType type;
    char      *permissions;
    char      *path;
    
} _VFFileInfo;
typedef _VFFileInfo * VFFileInfo;

// MARK: - VFFileUser -
typedef struct __VFFileUser {
    uint32_t user_id;
    char *   user_name;
    
} _VFFileUser;
typedef _VFFileUser * VFFileUser;

// MARK: - VFFileGroup -
typedef struct __VFFileGroup {
    uint32_t group_id;
    char *   group_name;
    
} _VFFileGroup;
typedef _VFFileGroup * VFFileGroup;

// MARK: - VFFileInfo Functions -
VFFileInfo VFFileInfoCreate(const char *path, char **error);
VFFileInfo VFFileInfoCopy(VFFileInfo info);

VFFileUser VFFileInfoCopyUser(VFFileInfo info, char **error);
VFFileGroup VFFileInfoCopyGroup(VFFileInfo info, char **error);

void VFFileUserRelease(VFFileUser user);
void VFFileGroupRelease(VFFileGroup group);
void VFFileInfoRelease(VFFileInfo info);


/*
 * =============================
 *    VFSystemInfo & Related
 * =============================
 *
 */
// MARK: - VFSystemInfo -

/*
 * Commas are inserted for readability
 *
 * NSFileSystemFreeNodes = 28,965,873;
 * NSFileSystemFreeSize  = 118,644,215,808;
 * NSFileSystemNodes     = 61,069,440;
 * NSFileSystemNumber    = 16777218;
 * NSFileSystemSize      = 250,140,434,432;
 *
 */

typedef struct __VFSystemInfo {
    uint64_t system_id;
    uint64_t max_file_length;
    uint64_t block_size;
    uint32_t blocks_count;
    uint32_t blocks_available;
    uint32_t blocks_free;
    uint32_t files_count;
    uint32_t files_available;
    uint32_t files_free;
} _VFSystemInfo;
typedef _VFSystemInfo * VFSystemInfo;

// MARK: - VFSystemInfo Functions -
VFSystemInfo VFSystemInfoCreate(const char *path, char **error);
void VFSystemInfoRelease(VFSystemInfo info);


/*
 * ====================================
 *    VFDiskDescription & Related
 * ====================================
 *
 */
// MARK: - VFDiskDescription -

typedef struct __VFDiskDescription {
    uint64_t system_id;
    uint64_t max_file_length;
    uint64_t bytes_total;
    uint64_t bytes_free;
    uint64_t bytes_available;
} _VFDiskDescription;
typedef _VFDiskDescription * VFDiskDescription;

// MARK: - VFSystemInfo Functions -
VFDiskDescription VFDiskDescriptionCreate(const char *path, char **error);
void VFDiskDescriptionRelease(VFDiskDescription info);

/*
 * =============================
 *     VFFileInfo & Related
 * =============================
 *
 */
// MARK: - File Management -
BOOL VFFileExists(const char *path);
BOOL VFFileIsDirectory(const char *path, char **error);

BOOL VFCreateDirectoryPermissions(const char *path, mode_t permissions, char **error);
BOOL VFCreateDirectory(const char *path, char **error);

BOOL VFMoveFile(const char *from, const char *to, char **error);
BOOL VFCopyFile(const char *from, const char *to, char **error); // Recursive copy of files / directories


/*
 * =============================
 *        File Scanning
 * =============================
 *
 */
// MARK: - File Scanning -
void VFEnumerateFileBuffer(const char *path, VFFileBytesEnumerationBlock block);


/*
 * =============================
 *     Direcotry Enumeration
 * =============================
 *
 */
// MARK: - Directory Enumeration -
void VFEnumerateDirectory(const char *path, VFFileEnumerationOption options, VFDirectoryEnumerationBlock block);











