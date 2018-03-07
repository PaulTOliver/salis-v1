CC      := gcc
BIN     := bin/tsalis

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
DEPS    := $(patsubst %.o,%.d,$(OBJECTS))

LFLAGS  := -L ../lib -lsalis -lncurses

# uncomment for debug
# OFLAGS  := -ggdb

# uncomment for release
OFLAGS  := -O3 -DNDEBUG

CFLAGS  := -Iinclude -I../include -c $(OFLAGS) -MMD -Wall -Wextra -std=c89 \
           -pedantic-errors -Wmissing-prototypes -Wstrict-prototypes \
           -Wold-style-definition

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BIN)

-include $(DEPS)

$(OBJECTS): $(patsubst build/%.o,src/%.c,$@)
	$(CC) $(CFLAGS) $(patsubst build/%.o,src/%.c,$@) -o $@

clean:
	-rm build/*
	-rm $(BIN)
