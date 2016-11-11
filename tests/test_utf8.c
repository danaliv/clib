#include "utf8.h"
#include "unity.h"

unichar ch;
int bytes;

void setUp() {
  ch = 0;
}

void test_decode_1() {
  bytes = utf8_next_char((uint8_t *) "beep", 4, &ch);
  TEST_ASSERT_EQUAL('b', ch);
  TEST_ASSERT_EQUAL(1, bytes);
}


void test_decode_2() {
  bytes = utf8_next_char((uint8_t *) "\xC3\xBEing", 5, &ch);
  TEST_ASSERT_EQUAL(0xfe, ch);
  TEST_ASSERT_EQUAL(2, bytes);
}

void test_decode_3() {
  bytes = utf8_next_char((uint8_t *) "\xE2\x86\x92 this way", 12, &ch);
  TEST_ASSERT_EQUAL(0x2192, ch);
  TEST_ASSERT_EQUAL(3, bytes);  
}


void test_decode_4() {
  bytes = utf8_next_char((uint8_t *) "\xF0\x9F\x98\xB8 meow", 9, &ch);
  TEST_ASSERT_EQUAL(0x1f638, ch);
  TEST_ASSERT_EQUAL(4, bytes);  
}

void test_short_2() {
  bytes = utf8_next_char((uint8_t *) "\xC3", 1, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);  
}

void test_short_3() {
  bytes = utf8_next_char((uint8_t *) "\xE3\x86", 2, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);  
}

void test_short_4() {
  bytes = utf8_next_char((uint8_t *) "\xF0\x9F\x98", 3, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);  
}

void test_undefined_bit_sequence_10() {
  bytes = utf8_next_char((uint8_t *) "\x80xyz", 4, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_undefined_bit_sequence_11111() {
  bytes = utf8_next_char((uint8_t *) "\xF8xyz", 4, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_missing_continuation_byte_2() {
  bytes = utf8_next_char((uint8_t *) "\xC3x", 2, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_missing_continuation_byte_3() {
  bytes = utf8_next_char((uint8_t *) "\xE2\x86x", 3, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_missing_continuation_byte_4() {
  bytes = utf8_next_char((uint8_t *) "\xF0\x9F\x98x", 4, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_overlong_encoding_2() {
  bytes = utf8_next_char((uint8_t *) "\xC0\xD7", 2, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_overlong_encoding_3() {
  bytes = utf8_next_char((uint8_t *) "\xE0\x80\xD7", 3, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}

void test_overlong_encoding_4() {
  bytes = utf8_next_char((uint8_t *) "\xF0\x80\x80\xD7", 4, &ch);
  TEST_ASSERT_EQUAL(-1, bytes);
}
