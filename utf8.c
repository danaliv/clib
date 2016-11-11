#include "utf8.h"

int utf8_next_char(uint8_t *text, size_t length, unichar *ch) {
  if (length == 0)
    return 0;
  
  if ((*text & 0x80) == 0) {
    *ch = *text;
    return 1;
  }
  
  if ((*text & 0xe0) == 0xc0 && length > 1 && (text[1] & 0xc0) == 0x80) {
    *ch = ((text[0] & 0x1f) << 6)
      | (text[1] & 0x3f);
    if (*ch >= 0x80)
      return 2;
  } else if ((*text & 0xf0) == 0xe0 && length > 2 &&
	     (text[1] & 0xc0) == 0x80 && (text[2] & 0xc0) == 0x80) {
    *ch = ((text[0] & 0xf) << 12)
      | ((text[1] & 0x3f) << 6)
      | (text[2] & 0x3f);
    if (*ch >= 0x800)
      return 3;
  } else if ((*text & 0xf8) == 0xf0 && length > 3 &&
	     (text[1] & 0xc0) == 0x80 && (text[2] & 0xc0) == 0x80 && (text[3] & 0xc0) == 0x80) {
    *ch = ((text[0] & 0x7) << 18)
      | ((text[1] & 0x3f) << 12)
      | ((text[2] & 0x3f) << 6)
      | (text[3] & 0x3f);
    if (*ch >= 0x10000 && *ch <= 0x10ffff)
      return 4;
  }
  
  return -1;
}
