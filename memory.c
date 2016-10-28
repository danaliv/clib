#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include "memory.h"

typedef struct {
    uint32_t refcount;
    uint32_t givecount; // deferred releases
    uint32_t len, capacity; // for buffers
    void (*onfree)(void*);
} ref_header_t;

int sizeof_ref_header_t = sizeof(ref_header_t); // test harness support

#define HDR(ref) ((ref_header_t *) ((ref) - sizeof(ref_header_t)))

void oom() {
    fprintf(stderr, "out of memory\n");
    exit(EX_OSERR);
}

ref_t alloc(size_t size) {
    ref_t ref = malloc(size + sizeof(ref_header_t));
    if (ref) {
        ref += sizeof(ref_header_t);
        HDR(ref)->refcount = 1;
        HDR(ref)->givecount = 0;
        HDR(ref)->onfree = NULL;
    }
    return ref;
}

ref_t alloc0(size_t size) {
    ref_t ref = calloc(size + sizeof(ref_header_t), 1);
    if (ref) {
        ref += sizeof(ref_header_t);
        HDR(ref)->refcount = 1;
        HDR(ref)->givecount = 0;
        HDR(ref)->onfree = NULL;
    }
    return ref;
}

ref_t xalloc(size_t size) {
    ref_t ref = alloc(size);
    if (!ref) oom();
    return ref;
}

ref_t xalloc0(size_t size) {
    ref_t ref = alloc0(size);
    if (!ref) oom();
    return ref;
}

buf_t bufalloc(uint32_t capacity) {
    buf_t buf = alloc(capacity);
    if (buf) {
        HDR(buf)->len = 0;
        HDR(buf)->capacity = capacity;
    }
    return buf;
}

buf_t xbufalloc(uint32_t capacity) {
    buf_t buf = xalloc(capacity);
    HDR(buf)->len = 0;
    HDR(buf)->capacity = capacity;
    return buf;
}

void onfree(ref_t ref, ref_cb cb) {
    HDR(ref)->onfree = cb;
}

ref_t retain(ref_t ref) {
    if (!ref) {
        return ref;
    }
    if (HDR(ref)->givecount > 0) {
        HDR(ref)->givecount--;
        return ref;
    }
    HDR(ref)->refcount++;
    return ref;
}

void release(ref_t ref) {
  if (!ref) {
        return;
    }
    HDR(ref)->refcount--;
    if (HDR(ref)->refcount == 0) {
        if (HDR(ref)->onfree) {
            HDR(ref)->onfree(ref);
        }
        free(HDR(ref));
    }
}

ref_t replace(ref_t ref, ref_t rep) {
    release(ref);
    return rep;
}

ref_t give(ref_t ref) {
    HDR(ref)->givecount++;
    return ref;
}

uint32_t buflen(buf_t buf) {
    return HDR(buf)->len;
}

bool bufensure(buf_t *bufp, size_t n) {
    if (HDR(*bufp)->capacity - HDR(*bufp)->len < n) {
        void *embiggened = realloc(
            *bufp - sizeof(ref_header_t),
            HDR(*bufp)->capacity * 2 + sizeof(ref_header_t)
        );

        if (embiggened) {
            *bufp = embiggened + sizeof(ref_header_t);
            HDR(*bufp)->capacity *= 2;
        } else {
            return false;
        }
    }
    return true;
}

bool bufappend(buf_t *bufp, const void *src, size_t n) {
    if (bufensure(bufp, n)) {
        memcpy(*bufp + HDR(*bufp)->len, src, n);
        HDR(*bufp)->len += n;
        return true;
    } else {
        return false;
    }
}

void xbufappend(buf_t *bufp, const void *src, size_t n) {
    if (!bufappend(bufp, src, n)) oom();
}

bool bufputc(buf_t *bufp, char c) {
    return bufappend(bufp, &c, 1);
}

void xbufputc(buf_t *bufp, char c) {
    if (!bufputc(bufp, c)) oom();
}

