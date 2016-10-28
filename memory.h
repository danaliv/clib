/*
 * Reference-counted memory allocation
 */

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * ref_t is a pointer to reference-counted memory. Use it like you would use
 * any other pointer. (But don't free() it, obviously. Call release() instead.)
 */
typedef void *ref_t;

/*
 * buf_t is a ref_t that can grow automatically to accommodate more data. Use
 * bufappend(), bufputc(), etc. for this feature. In all other respects, buf_t
 * is no different from ref_t: it is reference-counted and may be treated like
 * a normal pointer.
 */
typedef ref_t buf_t;

/*
 * The alloc functions are reference-counted equivalents to malloc(). The
 * resulting pointer has a reference count of one. The variants ending in 0
 * fill the allocated memory with zeroes; the variants beginning with x print
 * "out of memory" to stderr and exit with status EX_OSERR if allocation fails.
 *
 * The "buf" allocators reserve enough memory for <capacity> bytes, and return
 * a growable allocation. Use bufappend(), bufputc(), etc.
 */
ref_t alloc(size_t size);
ref_t alloc0(size_t size);
ref_t xalloc(size_t size);
ref_t xalloc0(size_t size);
buf_t bufalloc(uint32_t capacity);
buf_t xbufalloc(uint32_t capacity);

/*
 * Callbacks. Reference-counted allocations can optionally call a user function
 * before deallocation occurs. Only one callback can be set at a time. If
 * onfree() is called on a reference that already has a callback, the first
 * callback is replaced.
 */
typedef void (*ref_cb)(ref_t);
void onfree(ref_t ref, ref_cb cb);

/*
 * Reference counting - retain() increments an allocation's reference count and
 * returns its argument, and release() decrements it. If an allocation's
 * reference count reaches zero, the onfree callback (if set) is called and the
 * allocation is freed.
 *
 * replace() is a release-with-replacement. It releases an allocation and
 * returns another value (the replacement). This is useful for walking linked
 * lists and releasing them without having to set up temporary variables, e.g.
 * `node = replace(node, node->next)`.
 *
 * give() is a deferred release(). Instead of actually releasing the
 * reference, it causes the next retain() not to increment the reference count.
 * This is useful for allocating a reference and passing it to a function that
 * will retain it without having to release it right afterwards. give() can be
 * called multiple times in succession and it will increment the "give count"
 * each time.
 *
 * These functions are safe to call on NULL.
 */
ref_t retain(ref_t ref);
void release(ref_t ref);
ref_t replace(ref_t ref, ref_t rep);
ref_t give(ref_t ref);

/*
 * Buffers - buflen() returns the number of bytes stored in a buffer. (Not its
 * capacity.) bufappend() adds data to a buffer and increases its capacity if
 * necessary, returning true if the capacity increase is successful or
 * unnecessary. bufputc() is a special case for appending a single byte to a
 * buffer. Note that these functions may change the buffer's address. The
 * x-prefixed variants respond to capacity increase failures in the same manner
 * as xalloc.
 */
uint32_t buflen(buf_t buf);
bool bufappend(buf_t *bufp, const void *src, size_t n);
void xbufappend(buf_t *bufp, const void *src, size_t n);
bool bufputc(buf_t *bufp, char c);
void xbufputc(buf_t *bufp, char c);

#endif
