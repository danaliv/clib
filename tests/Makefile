.PHONY: all clean clib.a

CC?=gcc
CFLAGS?=-std=gnu99 -g -Wall -I.. -Iunity

test_sources=test_memory.c
test_deps=$(test_sources:.c=.d)
test_runners=$(test_sources:.c=_runner)
eris_objects=../memory.o

.SECONDARY: $(test_sources:.c=.o) $(test_runners:=.c) $(test_runners:=.o) unity/unity.o

all: clib.a $(test_runners)
	for runner in *_runner; do ./$$runner || exit 1; done

clib.a:
	$(MAKE) -C .. clib.a

%_runner.c: %.c
	ruby unity/generate_test_runner.rb $< $@

%_runner: %_runner.o %.o unity/unity.o $(eris_objects)
	$(CC) $(CFLAGS) -o $@ $^

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(test_deps)

clean:
	rm -f *.d *.o unity/unity.o *_runner.c *_runner
