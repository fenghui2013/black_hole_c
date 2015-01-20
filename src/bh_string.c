#include <stdlib.h>
#include <string.h>

#include "bh_string.h"

#define MEM_THRESHOLD 1024*64
#define MEM_STEP 1024*16

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
    string->start = 0;
    string->end = 0;

    return string;
}

void
bh_string_release(bh_string *string) {
    free(string->s);
    free(string);
}

static void
bh_string_expansion(bh_string *string) {
    char *new_s;
    int new_size;
    int i, j;

    if (string->size < MEM_THRESHOLD) {
        new_size = 2*string->size;
        new_s = (char *)malloc((new_size+2)*sizeof(char));
    } else {
        new_size = string->size+MEM_STEP;
        new_s = (char *)malloc((new_size+2)*sizeof(char));
    }

    if (string->start == 0) {
        free(string->s);
        string->s = new_s;
        string->size = new_size;
        string->free = new_size;
        string->start = 1;
    } else {
        for (i=1, j=string->start; j<=string->end; i++, j++) {
            new_s[i] = string->s[j];
        }
        free(string->s);
        string->s = new_s;
        string->size = new_size;
        string->start = 1;
        string->end = i-1;
        string->free = string->size-string->end;
    }
}

void
bh_string_set(bh_string *string, char *s, int start, int size, int appending) {
    int i, j;
    
    if (size <= 0) return;
    if (appending) {
        while (size > string->free) bh_string_expansion(string);
        for (i=string->end+1, j=start; j<size; i++, j++) {
            string->s[i] = s[j];
        }
        string->end = string->end + size;
        string->free = string->size-string->end;
    } else {
        while (size > string->size) bh_string_expansion(string);
        for (i=1, j=start; j<size; i++, j++) {
            string->s[i] = s[j];
        }
        string->start = 1;
        string->end = size;
        string->free = string->size-string->end;
    }
}
/*
 * -1, argument len invalid
 * 0, mem is valid
 * 1, mem is invalid
 */
int
bh_string_update(bh_string *string, int len) {
    if (len <= 0) return -1;
    string->start += len;
    if (string->start == string->size+1) {
        string->free = string->size;
        string->start = 0;
        string->end = 0;
        return 1;
    }
    return 0;
}

char *
bh_string_get(bh_string *string) {
    return string->s+string->start;
}

int
bh_string_get_size(bh_string *string) {
    return string->size;
}

int
bh_string_get_len(bh_string *string) {
    int len = string->end-string->start+1;
    if (string->start==0 && string->start==string->end) len = 0;
    return len;
}

int
bh_string_get_free(bh_string *string) {
    return string->free;
}
