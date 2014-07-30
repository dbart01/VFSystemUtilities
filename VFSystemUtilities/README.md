VFSystemUtilities
=================

A C-based library for file operations (copy, move, etc), hardware information access (whether its in iPhone 4 or iPhone 5C) and c-string tokenization

#### Hardware information
Easily access information about the current hardware with the following:
```c
const char *model = VFParameterCopyHardwareName(kVFHardwareStringMachineName, nil);
printf("Model: %s", model); // Model: iPhone6,1
free(model);
```

If more fine-grained control is required use one of the convenience functions:
```c
VFiPhoneType type = VFGetiPhoneType(nil);
if (type == VFiPhoneType5SGSM) {
  // You win!
}
```

#### Current Memory State
You have access to the current state of the device memory with just a single function call:
```c
VFMemorySnapshot snapshot = VFMemorySnapshotCreate(nil);
printf("Free memory: %llu bytes", snapshot->free_bytes); // Free memory: 160137216 bytes
VFMemorySnapshotRelease(snapshot);
```