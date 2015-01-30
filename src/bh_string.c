#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bh_string.h"

#define MEM_THRESHOLD 1024*16

struct bh_string {
    char *s;
    int size;  // total size
    int free;  // free size
    int start; // data start location
    int end;   // data end location 
};

bh_string *
bh_string_create(int size) {
    bh_string *string;

    string = (bh_string *)malloc(sizeof(bh_string));
    string->s = (char *)malloc((size+2)*sizeof(char));
    string->size = size;
    string->free = size;
    string->start = 1;
    string->end = 1;

    return string;
}

void
bh_string_release(bh_string *string) {
    free(string->s);
    free(string);
    string = NULL;
}

/*
 * 0, expansion fail 
 * 1, expansion success 
 */
int
bh_string_expansion(bh_string *string, int max_size) {
    char *new_s;
    int new_size;
    int i, j;

    // expansion fail
    if (string->size > max_size) return 0;

    new_size = 2*string->size;
    new_s = (char *)malloc((new_size+2)*sizeof(char));

    for (i=1, j=string->start; j<string->end; i++, j++) {
        new_s[i] = string->s[j];
    }
    free(string->s);
    string->s = new_s;
    string->size = new_size;
    string->start = 1;
    string->end = i-1;
    string->free = string->size-string->end+1;
    return 1;
}

/*
 * -1, argument len invalid
 * 0, mem is not reset
 * 1, mem is reset
 */
int
bh_string_update_start(bh_string *string, int len) {
    if (len <= 0) return -1;
    string->start += len;
    if (string->start == string->size+1) {
        string->free = string->size;
        string->start = 1;
        string->end = 1;
        return 1;
    }

    return 0;
}

/*
 * -1, argument len invalid
 * 0, normal
 */
int
bh_string_update_end(bh_string *string, int len) {
    if (len <= 0) return -1;
    string->end += len;
    string->free = string->size-string->end+1;

    return 0;
}

char *
bh_string_get_start(bh_string *string) {
    return string->s+string->start;
}

char *
bh_string_get_end(bh_string *string) {
    return string->s+string->end;
}

int
bh_string_get_size(bh_string *string) {
    return string->size;
}

int
bh_string_get_len(bh_string *string) {
    printf("bh_string_get_len start: %d, end:%d\n", string->start, string->end);
    return string->end-string->start;
}

int
bh_string_get_free(bh_string *string) {
    return string->free;
}
