AR      := ar
CC      := gcc
LIB     := lib/libsalis.a

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
DEPS    := $(patsubst %.o,%.d,$(OBJECTS))

LFLAGS  := rs

# uncomment for debug
# OFLAGS  := -ggdb

# uncomment for release
OFLAGS  := -O3 -DNDEBUG

CFLAGS  := -Iinclude -c $(OFLAGS) -MMD -Wall -Wextra -std=c89 -pedantic-errors \
           -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition

all: $(OBJECTS)
	$(AR) $(LFLAGS) $(LIB) $(OBJECTS)
	$(MAKE) -C tsalis

-include $(DEPS)

$(OBJECTS): $(patsubst build/%.o,src/%.c,$@)
	$(CC) $(CFLAGS) $(patsubst build/%.o,src/%.c,$@) -o $@

clean:
	-rm build/*
	-rm $(LIB)
	$(MAKE) clean -C tsalis
