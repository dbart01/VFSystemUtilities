//
//  VFTokenCollection.m
//
//  Created by Dima Bart on 2014-03-14.
//  Copyright (c) 2014 Dima Bart. All rights reserved.
//

#import "VFTokenCollection.h"

//static char ** VFStringTokenize(const char *string, const char *separator, int *count) {
//    int token_count = 0;
//    char *tokens_array[512];
//
//    char *reference;
//    char *stack   = strdup(string);
//    char *token   = strtok_r(stack, separator, &reference);
//    while (token) {
//
//        if (strlen(token) > 0) {
//            tokens_array[token_count] = token;
//            token_count++;
//        }
//
//        token = strtok_r(NULL, separator, &reference);
//    }
//    free(stack);
//
//    *count = token_count;
//
//    size_t size       = sizeof(char *) * token_count;
//    char **collection = malloc(size);
//    memcpy(collection, tokens_array, size);
//
//    return collection;
//}
//
//static void VFFreeArray(void **array, int count) {
//    for (int i=0;i<count;i++) {
//        free(array[i]);
//    }
//    free(array);
//}

#pragma mark - VFTokenCollection -
VFTokenCollection VFTokenCollectionCreate(const char *string, const char *separator) {
    int token_count = 0;
    char *tokens_array[512];
    
    char *reference;
    char *stack   = strdup(string);
    char *token   = strtok_r(stack, separator, &reference);
    while (token) {
        
        if (strlen(token) > 0) {
            tokens_array[token_count] = strdup(token);
            token_count++;
        }
        
        token = strtok_r(NULL, separator, &reference);
    }
    free(stack);
    
    
    size_t size       = sizeof(char *) * token_count;
    char **collection = malloc(size);
    memcpy(collection, tokens_array, size);
    
    VFTokenCollection token_collection = malloc(sizeof(struct VFTokenCollection));
    token_collection->tokens           = collection;
    token_collection->count            = token_count;
    
    return token_collection;
}

void VFTokenCollectionRelease(VFTokenCollection collection) {
    for (int i=0;i<collection->count;i++) {
        free(collection->tokens[i]);
    }
    free(collection->tokens);
    free(collection);
}

char * VFTokenCollectionGetLastComponent(VFTokenCollection collection) {
    return collection->tokens[collection->count - 1];
}

//const char *path = "/User/This/Is/Really/Fun/Because/I/Get/Tok/Make/ACool/List/Of/Thing/WIth/A/Really/Long/Path/Name/That/Suits/Cake/Muffins/And/Pies";
//for (int i=0;i<1000000;i++) {
//
//    VFTokenCollection collection = VFTokenCollectionCreate(path, "/");
//    for (int i=0;i<collection->count;i++) {
//
//    }
//    VFTokenCollectionRelease(collection);
//}

#pragma mark - String Operations -
char * VFJoin(const char *str1, const char *str2) {
    size_t len1  = strlen(str1);
    size_t len2  = strlen(str2);
    char *result = malloc(len1 + len2 + 2);
    memcpy(result, str1, len1);
    memcpy(result + len1, str2, len2 + 1);
    
    return result;
}

int VFCompare(const char *string1, const char *string2) {
    return (strcmp(string1, string2) == 0);
}














