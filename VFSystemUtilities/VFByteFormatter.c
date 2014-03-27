//
//  VFByteFormatter.m
//
//  Created by Dima Bart on 2014-03-17.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import "VFByteFormatter.h"

static double _base         = 1024.0f;
static const char *_sizes[] = {
    "B",
    "KB",
    "MB",
    "GB",
    "TB"
};

const char * VFCreateByteFormat(uint8_t bytes, int use_space) {
    int power           = 0;
    double bytes_double = 1.0f * bytes;
    while (bytes_double > _base) {
        power++;
        bytes_double /= _base;
    }
    
    if (bytes > 0) {
        char *string;
        char *space = (use_space) ? " " : "";
        asprintf(&string, "%0.1f%s%s", bytes_double, space, _sizes[power]);
        
        return string;
    }
    return NULL;
}
