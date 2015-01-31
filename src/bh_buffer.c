#include <stdlib.h>

#include "bh_string.h"
#include "bh_buffer.h"

typedef struct bh_buffer_node bh_buffer_node;
struct bh_buffer_node {
    bh_string *string;
    bh_buffer_node *next;
    int reuse_count;
};

struct bh_buffer {
    bh_buffer_node *first;
    bh_buffer_node *current_read;
    bh_buffer_node *current_write;
    bh_buffer_node *last;
    int count;
    int free_count;
    int size;
    int max_size;
};

bh_buffer_node *
_bh_buffer_node_create(int size) {
    bh_buffer_node *node = (bh_buffer_node *)malloc(sizeof(bh_buffer_node));
    bh_string *string = bh_string_create(size);
    node->string = string;
    node->reuse_count = 0;
    node->next = NULL;

    return node;
}

bh_buffer *
bh_buffer_create(int size, int max_size) {
    bh_buffer *buffer = (bh_buffer *)malloc(sizeof(bh_buffer));

    bh_buffer_node *node = _bh_buffer_node_create(size);

    buffer->size = size;
    buffer->max_size = max_size;
    buffer->count = 1;
    buffer->free_count = 0;
    buffer->first = node;
    buffer->current_read = node;
    buffer->current_write = node;
    buffer->last = node;

    return buffer;
}

void
_bh_buffer_node_release(bh_buffer_node *node) {
    bh_string_release(node->string);
    free(node);
    node = NULL;
}

void
bh_buffer_release(bh_buffer *buffer) {
    bh_buffer_node *temp, *node;

    node = buffer->first;
    temp = buffer->first;
    while (node) {
        node = node->next;
        _bh_buffer_node_release(temp);
        temp = node;
    }
    free(buffer);
    buffer = NULL;
}

int
bh_buffer_get_write(bh_buffer *buffer, char **s) {
    int free = bh_string_get_free(buffer->current_write->string);
    int is_expansion = 1;

    if (free == 0) {
        is_expansion = bh_string_expansion(buffer->current_write->string, buffer->max_size);
        free = bh_string_get_free(buffer->current_write->string);
    }

    // expansion failed
    if (is_expansion == 0) {
        if (buffer->free_count > 0) {
            buffer->current_write = buffer->first;
            buffer->first = buffer->first->next;
            buffer->free_count -= 1;
            buffer->current_write->reuse_count += 1;
        } else {
            buffer->current_write = _bh_buffer_node_create(buffer->size);
            buffer->count += 1;
        }
        buffer->last->next = buffer->current_write;
        buffer->last = buffer->current_write;
        buffer->last->next = NULL;
        free = bh_string_get_free(buffer->current_write->string);
    }
    *s = bh_string_get_end(buffer->current_write->string);
    return free;
}

void
bh_buffer_set_write(bh_buffer *buffer, int len) {
    bh_string_update_end(buffer->current_write->string, len);
}

int
bh_buffer_get_read(bh_buffer *buffer, char **s) {
    int len = bh_string_get_len(buffer->current_read->string);
    *s = bh_string_get_start(buffer->current_read->string);

    return len;
}

void
bh_buffer_set_read(bh_buffer *buffer, int len) {
    int reset = bh_string_update_start(buffer->current_read->string, len);
    if (reset) {
        if (buffer->current_read != buffer->current_write) {
            buffer->current_read = buffer->current_read->next;
            buffer->free_count += 1;
        }
    }
}

int
bh_buffer_get_count(bh_buffer *buffer) {
    return buffer->count;
}

int
bh_buffer_get_free_count(bh_buffer *buffer) {
    return buffer->free_count;
}

int
bh_buffer_get_size(bh_buffer *buffer) {
    int size = 0;
    bh_buffer_node *node = buffer->first;

    while (node) {
        size += bh_string_get_size(node->string);
        node = node->next;
    }

    return size;
}

int
bh_buffer_get_reuse_count(bh_buffer *buffer) {
    int reuse_count = 0;
    bh_buffer_node *node = buffer->first;

    while (node) {
        reuse_count += node->reuse_count;
        node = node->next;
    }

    return reuse_count;
}

int
bh_buffer_get_reuse_size(bh_buffer *buffer) {
    int reuse_size = 0;
    bh_buffer_node *node = buffer->first;

    while (node) {
        reuse_size += node->reuse_count*bh_string_get_size(node->string);
        node = node->next;
    }

    return reuse_size;
}
