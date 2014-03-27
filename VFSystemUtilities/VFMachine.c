//
//  VFMemoryStatus.m
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

VFDeviceType VFGetDeviceType(char **error) {
    const char *device_name = VFCopyDeviceName(error);
    VFDeviceType type       = VFDeviceTypeUndetermined;
    if (device_name) {
        if (VFContains(device_name, "iPhone")) type = VFDeviceTypeiPhone;
        if (VFContains(device_name, "iPad"))   type = VFDeviceTypeiPad;
        if (VFContains(device_name, "iPod"))   type = VFDeviceTypeiPod;
    }
    
    free((char *)device_name);
    return type;
}

VFiPhoneType VFGetiPhoneType(char **error) {
    const char *device_name = VFCopyDeviceName(error);
    VFiPhoneType type       = VFiPhoneTypeUndetermined;
    if (device_name) {
        if (VFCompare(device_name, "iPhone1,1")) type = VFiPhoneTypeOriginal;
        if (VFCompare(device_name, "iPhone1,2")) type = VFiPhoneType3G;
        if (VFCompare(device_name, "iPhone2,1")) type = VFiPhoneType3GS;
        if (VFCompare(device_name, "iPhone3,1")) type = VFiPhoneType4GSM;
        if (VFCompare(device_name, "iPhone3,2")) type = VFiPhoneType4Budget;
        if (VFCompare(device_name, "iPhone3,3")) type = VFiPhoneType4CDMA;
        if (VFCompare(device_name, "iPhone4,1")) type = VFiPhoneType4S;
        if (VFCompare(device_name, "iPhone5,1")) type = VFiPhoneType5GSM;
        if (VFCompare(device_name, "iPhone5,2")) type = VFiPhoneType5Dual;
        if (VFCompare(device_name, "iPhone5,3")) type = VFiPhoneType5CDual;
        if (VFCompare(device_name, "iPhone5,4")) type = VFiPhoneType5CCDMA;
        if (VFCompare(device_name, "iPhone6,1")) type = VFiPhoneType5SGSM;
        if (VFCompare(device_name, "iPhone6,2")) type = VFiPhoneType5SDual;
    }
    
    free((char *)device_name);
    return type;
}

VFiPadType VFGetiPadType(char **error) {
    const char *device_name = VFCopyDeviceName(error);
    VFiPadType type         = VFiPadTypeUndetermined;
    if (device_name) {
        if (VFCompare(device_name, "iPad1,1")) type = VFiPadTypeOriginal;
        if (VFCompare(device_name, "iPad2,1")) type = VFiPadType2WiFi;
        if (VFCompare(device_name, "iPad2,2")) type = VFiPadType2GSM;
        if (VFCompare(device_name, "iPad2,3")) type = VFiPadType2CDMA;
        if (VFCompare(device_name, "iPad2,4")) type = VFiPadType2WiFi;
        if (VFCompare(device_name, "iPad2,5")) type = VFiPadTypeMiniWifi;
        if (VFCompare(device_name, "iPad2,6")) type = VFiPadTypeMiniGSM;
        if (VFCompare(device_name, "iPad2,7")) type = VFiPadTypeMiniMM;
        if (VFCompare(device_name, "iPad3,1")) type = VFiPadType3Wifi;
        if (VFCompare(device_name, "iPad3,2")) type = VFiPadType3Verizon;
        if (VFCompare(device_name, "iPad3,3")) type = VFiPadType3GSM;
        if (VFCompare(device_name, "iPad3,4")) type = VFiPadType4Wifi;
        if (VFCompare(device_name, "iPad3,5")) type = VFiPadType4GSM;
        if (VFCompare(device_name, "iPad3,6")) type = VFiPadType4MM;
        if (VFCompare(device_name, "iPad4,1")) type = VFiPadTypeAirWifi;
        if (VFCompare(device_name, "iPad4,2")) type = VFiPadTypeAirGSM;
        if (VFCompare(device_name, "iPad4,3")) type = VFiPadTypeMystery;
        if (VFCompare(device_name, "iPad4,4")) type = VFiPadTypeMini2Wifi;
        if (VFCompare(device_name, "iPad4,5")) type = VFiPadTypeMini2GSM;
    }
    
    free((char *)device_name);
    return type;
}

VFiPodType VFGetiPodType(char **error) {
    const char *device_name = VFCopyDeviceName(error);
    VFiPodType type         = VFiPodTypeUndetermined;
    if (device_name) {
        if (VFCompare(device_name, "iPod1,1")) type = VFiPodTypeOriginal;
        if (VFCompare(device_name, "iPod2,1")) type = VFiPodType2G;
        if (VFCompare(device_name, "iPod3,1")) type = VFiPodType3G;
        if (VFCompare(device_name, "iPod4,1")) type = VFiPodType4G;
        if (VFCompare(device_name, "iPod5,1")) type = VFiPodType5G;
    }
    
    free((char *)device_name);
    return type;
}

















