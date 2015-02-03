#ifndef _BH_BUFFER_H_
#define _BH_BUFFER_H_

typedef struct bh_buffer_node bh_buffer_node;
typedef struct bh_buffer bh_buffer;

bh_buffer *  bh_buffer_create(int size, int max_size);
void         bh_buffer_release(bh_buffer *buffer);
int          bh_buffer_get_write(bh_buffer *buffer, char **s);
void         bh_buffer_set_write(bh_buffer *buffer, int len);
int          bh_buffer_get_read(bh_buffer *buffer, char **s);
void         bh_buffer_set_read(bh_buffer *buffer, int len);
int          bh_buffer_get_count(bh_buffer *buffer);
int          bh_buffer_get_free_count(bh_buffer *buffer);
int          bh_buffer_get_size(bh_buffer *buffer);
int          bh_buffer_get_reuse_count(bh_buffer *buffer);
int          bh_buffer_get_reuse_size(bh_buffer *buffer);
#endif
