//
//  VFMemoryStatus.m
//  FileIO
//
//  Created by Dima Bart on 2014-03-17.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import "VFMachine.h"
#import "VFTokenCollection.h"

#pragma mark - Private -
const char * VFHardwareCopyName(const char *name, char **error) {
    size_t size;
    sysctlbyname(name, NULL, &size, NULL, 0);
    char *buffer = malloc(size);
    if (sysctlbyname(name, buffer, &size, NULL, 0) == -1) {
        if (error) {
            *error = strerror(errno);
        }
    }
    return buffer;
}

uint64_t VFHardwareGetNumber(const char *name, char **error) {
    size_t size     = sizeof(uint64_t);
    uint64_t buffer = 0;
    if (sysctlbyname(name, &buffer, &size, NULL, 0) == -1) {
        if (error) {
            *error = strerror(errno);
        }
    }
    return buffer;
}

uint64_t VFMemoryCopyPagesize(char **error) {
    return VFHardwareGetNumber("hw.pagesize", error);
}

vm_statistics_data_t VFMemoryGetStat(char **error) {
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t info;
    if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&info, &count) != KERN_SUCCESS) {
        if (error) {
            *error = strerror(errno);
        }
    }
    return info;
}

#pragma mark - VFMemorySnapshot -
VFMemorySnapshot VFMemorySnapshotCreate(char **error) {
    if (error) *error = NULL;
    
    uint64_t pagesize = VFMemoryCopyPagesize(error);
    if (*error) return NULL;
    
    vm_statistics_data_t info = VFMemoryGetStat(error);
    if (*error) return NULL;
    
    VFMemorySnapshot snapshot = malloc(sizeof(_VFMemorySnapshot));
    if (snapshot) {
        snapshot->total_bytes    = (info.wire_count + info.active_count + info.inactive_count + info.free_count) * pagesize;
        snapshot->wired_bytes    = info.wire_count * pagesize;
        snapshot->active_bytes   = info.active_count * pagesize;
        snapshot->inactive_bytes = info.inactive_count * pagesize;
        snapshot->free_bytes     = info.free_count * pagesize;
        
        return snapshot;
    }
    return NULL;
}

void VFMemorySnapshotRelease(VFMemorySnapshot snapshot) {
    if (snapshot) {
        free(snapshot);
    }
}

#pragma mark - VFParameters -
const char * VFParameterCopyHardwareName(const char *name, char **error) {
    return VFHardwareCopyName(name, error);
}

uint64_t VFParameterGetHardwareNumber(const char *name, char **error) {
    return VFHardwareGetNumber(name, error);
}

#pragma mark - Device Types -
const char * VFCopyDeviceName(char **error) {
    return VFParameterCopyHardwareName("hw.machine", error);
}

VFiPhoneType VFGetiPhoneType(char **error) {
    const char *device_name = VFCopyDeviceName(error);
    if (device_name) {
        if (VFCompare(device_name, "iPhone1,1")) return VFiPhoneTypeOriginal;
        if (VFCompare(device_name, "iPhone1,2")) return VFiPhoneType3G;
        if (VFCompare(device_name, "iPhone2,1")) return VFiPhoneType3GS;
        if (VFCompare(device_name, "iPhone3,1")) return VFiPhoneType4GSM;
        if (VFCompare(device_name, "iPhone3,2")) return VFiPhoneType4Budget;
        if (VFCompare(device_name, "iPhone3,3")) return VFiPhoneType4CDMA;
        if (VFCompare(device_name, "iPhone4,1")) return VFiPhoneType4S;
        if (VFCompare(device_name, "iPhone5,1")) return VFiPhoneType5GSM;
        if (VFCompare(device_name, "iPhone5,2")) return VFiPhoneType5Dual;
        if (VFCompare(device_name, "iPhone5,3")) return VFiPhoneType5CDual;
        if (VFCompare(device_name, "iPhone5,4")) return VFiPhoneType5CCDMA;
        if (VFCompare(device_name, "iPhone6,1")) return VFiPhoneType5SGSM;
        if (VFCompare(device_name, "iPhone6,2")) return VFiPhoneType5SDual;
    }
    return VFiPhoneTypeUndetermined;
}

VFiPadType VFGetiPadType(char **error) {
    const char *device_name = VFCopyDeviceName(error);
    if (device_name) {
        if (VFCompare(device_name, "iPad1,1")) return VFiPadTypeOriginal;
        if (VFCompare(device_name, "iPad2,1")) return VFiPadType2WiFi;
        if (VFCompare(device_name, "iPad2,2")) return VFiPadType2GSM;
        if (VFCompare(device_name, "iPad2,3")) return VFiPadType2CDMA;
        if (VFCompare(device_name, "iPad2,4")) return VFiPadType2WiFi;
        if (VFCompare(device_name, "iPad2,5")) return VFiPadTypeMiniWifi;
        if (VFCompare(device_name, "iPad2,6")) return VFiPadTypeMiniGSM;
        if (VFCompare(device_name, "iPad2,7")) return VFiPadTypeMiniMM;
        if (VFCompare(device_name, "iPad3,1")) return VFiPadType3Wifi;
        if (VFCompare(device_name, "iPad3,2")) return VFiPadType3Verizon;
        if (VFCompare(device_name, "iPad3,3")) return VFiPadType3GSM;
        if (VFCompare(device_name, "iPad3,4")) return VFiPadType4Wifi;
        if (VFCompare(device_name, "iPad3,5")) return VFiPadType4GSM;
        if (VFCompare(device_name, "iPad3,6")) return VFiPadType4MM;
        if (VFCompare(device_name, "iPad4,1")) return VFiPadTypeAirWifi;
        if (VFCompare(device_name, "iPad4,2")) return VFiPadTypeAirGSM;
        if (VFCompare(device_name, "iPad4,3")) return VFiPadTypeMystery;
        if (VFCompare(device_name, "iPad4,4")) return VFiPadTypeMini2Wifi;
        if (VFCompare(device_name, "iPad4,5")) return VFiPadTypeMini2GSM;
    }
    return VFiPadTypeUndetermined;
}

















