#ifndef _BH_STRING_H_

typedef struct bh_string bh_string;

bh_string * bh_string_create(int size);
void        bh_string_release(bh_string *string);
void        bh_string_set(bh_string *string, char *s, int start, int size, int appending);
char *      bh_string_get(bh_string *string);
int         bh_string_get_size(bh_string *string);
int         bh_string_get_len(bh_string *string);
int         bh_string_get_free(bh_string *string);
#endif
