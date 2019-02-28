#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static uint8_t *rb_top, *rb_bottom, *rb_read, *rb_write;

bool rb_init(size_t size) {
    rb_top = malloc(size);
    if(rb_top != NULL) {
        // malloc was successful
        rb_read = rb_top;
        rb_write = rb_top;
        rb_bottom = rb_top + size;
        return true;
    }
    else {
        // malloc was unsuccessful
        // heckin' RIP
        return false;
    }
}

size_t rb_getBufferHealth() {
    
}

