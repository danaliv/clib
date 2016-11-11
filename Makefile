.PHONY: all clean test

CC?=gcc
CFLAGS?=-std=gnu99 -g -Wall

sources=memory.c utf8.c
objects=$(sources:.c=.o)
deps=$(sources:.c=.d)

all: clib.a test

clib.a: $(objects)
	ar rs clib.a $(objects)

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(deps)

test:
	$(MAKE) -C tests

lint: 
	splint +skip-sys-headers +posixlib -exitarg $(sources)

clean:
	rm -f clib.a *.o *.d
	$(MAKE) -C tests clean
