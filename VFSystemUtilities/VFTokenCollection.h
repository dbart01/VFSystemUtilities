//
//  VFTokenCollection.h
//
//  Created by Dima Bart on 2014-03-14.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import <stdlib.h>
#import <stdio.h>
#import <string.h>

/*
 * =============================
 *      VFTokenCollection
 * =============================
 *
 */
#pragma mark - VFTokenCollection -
struct VFTokenCollection {
    int count;
    char **tokens;
};
typedef struct VFTokenCollection * VFTokenCollection;

VFTokenCollection VFTokenCollectionCreate(const char *string, const char *separator);
char * VFTokenCollectionGetLastComponent(VFTokenCollection collection);
void VFTokenCollectionRelease(VFTokenCollection collection);

#pragma mark - String Operations -
char * VFJoin(const char *str1, const char *str2);
int VFCompare(const char *string1, const char *string2);
int VFContains(const char *string1, const char *string2);