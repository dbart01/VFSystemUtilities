//
//  VFMemoryStatus.h
//  FileIO
//
//  Created by Dima Bart on 2014-03-17.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import <stdlib.h>
#import <stdio.h>
#import <errno.h>
#import <sys/sysctl.h>
#import <mach/mach_host.h>
#import <mach/host_info.h>

/*
 * =============================
 *  VFMemorySnapshot & Related
 * =============================
 *
 */
// MARK: - VFMemorySnapshot -
typedef struct __VFMemorySnapshot {
    uint64_t total_bytes;
    uint64_t wired_bytes;
    uint64_t active_bytes;
    uint64_t inactive_bytes;
    uint64_t free_bytes;
} _VFMemorySnapshot;
typedef _VFMemorySnapshot * VFMemorySnapshot;

// MARK: - VFMemorySnapshot Functions -
VFMemorySnapshot VFMemorySnapshotCreate(char **error);
void VFMemorySnapshotRelease(VFMemorySnapshot snapshot);

/*
 * =============================
 *    Hardware Number Types
 * =============================
 *
 */
static const char *kVFHardwareNumberCPUCount      = "hw.ncpu";
static const char *kVFHardwareNumberByteOrder     = "hw.byteorder";
static const char *kVFHardwareNumberPhysicalMem   = "hw.physmem";
static const char *kVFHardwareNumberUserMem       = "hw.usermem";
static const char *kVFHardwareNumberPagesize      = "hw.pagesize";
static const char *kVFHardwareNumberEpoch         = "hw.epoch";
static const char *kVFHardwareNumberFloat         = "hw.floatingpoint";
static const char *kVFHardwareNumberVectorUnit    = "hw.vectorunit";
static const char *kVFHardwareNumberBusFrequency  = "hw.busfrequency";
static const char *kVFHardwareNumberCPUFrequency  = "hw.cpufrequency";
static const char *kVFHardwareNumberCacheLineSize = "hw.cachelinesize";
static const char *kVFHardwareNumberL1CacheSize   = "hw.l1icachesize";
static const char *kVFHardwareNumberL1DCacheSize  = "hw.l1dcachesize";
static const char *kVFHardwareNumberL2CacheSize   = "hw.l2cachesize";
static const char *kVFHardwareNumberL2Settings    = "hw.l2settings";
static const char *kVFHardwareNumberL3CacheSize   = "hw.l3cachesize";
static const char *kVFHardwareNumberL3Settings    = "hw.l3settings";
static const char *kVFHardwareNumberTBFrequency   = "hw.tbfrequency";
static const char *kVFHardwareNumberCPUAvailable  = "hw.availcpu";

/*
 * =============================
 *    Hardware String Types
 * =============================
 *
 */
static const char *kVFHardwareStringMachineName = "hw.machine";
static const char *kVFHardwareStringMachineArch = "hw.machinearch";
static const char *kVFHardwareStringModelName   = "hw.model";

/*
 * =============================
 *       iOS Device Types
 * =============================
 *
 */
typedef enum {
    VFiPhoneTypeUndetermined = 0,
    VFiPhoneTypeOriginal     = 1,
    VFiPhoneType3G           = 2,
    VFiPhoneType3GS          = 3,
    VFiPhoneType4GSM         = 4,
    VFiPhoneType4Budget      = 5,
    VFiPhoneType4CDMA        = 6,
    VFiPhoneType4S           = 7,
    VFiPhoneType5GSM         = 8,
    VFiPhoneType5Dual        = 9,
    VFiPhoneType5CDual       = 10,
    VFiPhoneType5CCDMA       = 11,
    VFiPhoneType5SGSM        = 12,
    VFiPhoneType5SDual       = 13,
} VFiPhoneType;

typedef enum {
    VFiPadTypeUndetermined = 0,
    VFiPadTypeOriginal     = 1,
    VFiPadType2WiFi        = 2,
    VFiPadType2GSM         = 3,
    VFiPadType2CDMA        = 4,
    VFiPadTypeMiniWifi     = 5,
    VFiPadTypeMiniGSM      = 6,
    VFiPadTypeMiniMM       = 7,
    VFiPadType3Wifi        = 8,
    VFiPadType3GSM         = 9,
    VFiPadType3Verizon     = 10,
    VFiPadType4Wifi        = 11,
    VFiPadType4GSM         = 12,
    VFiPadType4MM          = 13,
    VFiPadTypeAirWifi      = 14,
    VFiPadTypeAirGSM       = 15,
    VFiPadTypeMystery      = 16,
    VFiPadTypeMini2Wifi    = 17,
    VFiPadTypeMini2GSM     = 18,
} VFiPadType;



/*
 * =============================
 *        VFParameters
 * =============================
 *
 */
const char * VFParameterCopyHardwareName(const char *name, char **error);
uint64_t VFParameterGetHardwareNumber(const char *name, char **error);

/*
 * =============================
 *     Convenience Functions
 * =============================
 *
 */
const char * VFCopyDeviceName(char **error);
VFiPhoneType VFGetiPhoneType();
VFiPadType VFGetiPadType();








