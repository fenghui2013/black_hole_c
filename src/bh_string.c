#include <stdlib.h>

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
    string.s = (char *)malloc(size*sizeof(char));
    string.size = size;
    string.free = size;
    string.start = 0;
    string.end = 0;

    return string;
}

void
bh_string_release(bh_string *string) {
    free(string.s);
    free(string);
}

static void
bh_string_expansion(bh_string *sting) {
    char *new_s;
    int new_size;
    int i, j;

    if (string.size < MEM_THRESHOLD) {
        new_size = 2*string.size;
        new_s = (char *)malloc(new_size*sizeof(char));
    } else {
        new_size = string.size+MEM_STEP;
        new_s = (char *)malloc(new_size*sizeof(char));
    }

    for (i=0, j=string.start; j<string.end; i++, j++) {
        new_s[i] = string.s[j];
    }
    free(string.s);
    string.s = new_s;
    string.size = new_size;
    string.start = 0;
    string.end = i;
}

void
bh_string_set(bh_string *string, char *s, int start, int size, int appending) {
    int i, j;

    if (appending) {
        while (size > string.free) bh_string_expansion(string);
        for (i=string.end, j=start; j<size; i++, j++) {
            string.s[i] = s[j];
        }
        string.end = string.end + size;
        string.free = string.free - size;
    } else {
        while (size > string.size) bh_string_expansion(string);
        for (i=0, j=start; j<size; i++, j++) {
            string.s[i] = s[j];
        }
        string.start = 0;
        string.end = size;
        string.free = string.size - string.len;
    }
}

char *
bh_string_get(bh_string *string) {
    return string.s;
}

int
bh_string_get_size(bh_string *string) {
    return string.size;
}

int
bh_string_get_len(bh_string *string) {
    return string.end - string.start;
}

int
bh_string_get_free(bh_string *string) {
    return string.free;
}
