#ifndef _BH_STRING_H_

typedef struct bh_string bh_string;

bh_string * bh_string_create(int size);
void        bh_string_release(bh_string *string);
void        bh_string_expansion(bh_string *string, int max_size);
int         bh_string_update_start(bh_string *string, int len);
void        bh_string_update_end(bh_string *string, int len);
char *      bh_string_get(bh_string *string);
int         bh_string_get_size(bh_string *string);
int         bh_string_get_len(bh_string *string);
int         bh_string_get_free(bh_string *string);
#endif
