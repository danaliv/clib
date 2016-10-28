#include <stdbool.h>
#include <string.h>
#include "memory.h"
#include "unity.h"

#ifdef __linux__
#   include <malloc.h>
#   define MALLOC_SIZE malloc_usable_size
#else
#   include <malloc/malloc.h>
#   define MALLOC_SIZE malloc_size
#endif

extern int sizeof_ref_header_t;

bool freed;

void setUp() {
    freed = false;
}

void onfree_cb(ref_t ref) {
    freed = true;
}

void test_alloc() {
    ref_t ref = alloc(64);
    size_t size = MALLOC_SIZE(ref - sizeof_ref_header_t);
    release(ref);
    TEST_ASSERT_TRUE(size >= 64 + sizeof_ref_header_t);
}

void test_alloc0() {
    ref_t ref = alloc0(8);
    TEST_ASSERT_EQUAL(0, memcmp("\0\0\0\0\0\0\0\0", ref, 8));
}

void test_retain_release() {
    ref_t ref = alloc(64);
    onfree(ref, onfree_cb);
    retain(ref);
    release(ref);
    TEST_ASSERT_FALSE(freed);
    release(ref);
    TEST_ASSERT_TRUE(freed);
}

void test_replace() {
    ref_t a = alloc(64);
    ref_t b = alloc(64);
    onfree(a, onfree_cb);
    ref_t b2 = replace(a, b);
    TEST_ASSERT_TRUE(freed);
    TEST_ASSERT_EQUAL(b, b2);
    release(b);
}

void test_give() {
    ref_t ref = alloc(64);
    onfree(ref, onfree_cb);
    give(ref);
    TEST_ASSERT_FALSE(freed);
    retain(ref);
    release(ref);
    TEST_ASSERT_TRUE(freed);
}

void test_bufappend() {
    buf_t buf = bufalloc(8);
    static const char *hello = "hello world";
    bufappend(&buf, hello, strlen(hello));
    TEST_ASSERT_EQUAL_STRING_LEN(hello, buf, strlen(hello));
    static const char *howru = ", how are you?";
    bufappend(&buf, howru, strlen(howru) + 1);
    TEST_ASSERT_EQUAL_STRING("hello world, how are you?", buf);
    release(buf);
}

void test_bufputc() {
    buf_t buf = bufalloc(8);
    bufputc(&buf, 'f');
    bufputc(&buf, 'o');
    bufputc(&buf, 'o');
    bufputc(&buf, 'b');
    bufputc(&buf, 'a');
    bufputc(&buf, 'r');
    bufputc(&buf, 'b');
    bufputc(&buf, 'a');
    bufputc(&buf, 'z');
    bufputc(&buf, '\0');
    TEST_ASSERT_EQUAL_STRING("foobarbaz", buf);
}
