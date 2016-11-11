/*
 *  UTF-8 decoding
 */

#ifndef __UTF8_H__
#define __UTF8_H__

#include <stdint.h>
#include <sys/types.h>

/*
 * unichar is a character type wide enough to hold any Unicode code point.
 */
typedef uint32_t unichar;

/*
 * utf8_next_char decodes the next character in a byte stream encoded with
 * UTF-8. The decoded character is stored in <ch>, and the length of the
 * character's encoding in bytes is returned. <text> should point to the
 * current position in the stream, and <length> should be the number of bytes
 * remaining in the stream.
 *
 * If a decoding error occurs, utf8_next_char returns a value less than zero.
 * Decoding is strict: overlong sequences are rejected, as are characters
 * outside the Unicode code space.
 */
int utf8_next_char(uint8_t *text, size_t length, unichar *ch);

#endif // __UTF8_H__

