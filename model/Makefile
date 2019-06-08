CC = gcc
LIB_DEB = ../bin/libsalis-deb.so
LIB_REL = ../bin/libsalis-rel.so
SOURCES = $(wildcard *.c)
OBJECTS_DEB = $(patsubst %.c,../build/debug/%.o,$(SOURCES))
OBJECTS_REL = $(patsubst %.c,../build/release/%.o,$(SOURCES))
DEPS_DEB = $(patsubst %.o,%.d,$(OBJECTS_DEB))
DEPS_REL = $(patsubst %.o,%.d,$(OBJECTS_REL))

# Linker flags
LFLAGS = -shared

# Compiler flags for debug build.
DEB_FLAGS = -ggdb

# Compiler flags for release build.
REL_FLAGS = -O3 -DNDEBUG

# General compiler flags.
CFLAGS = -c -MMD -Wall -Wextra -std=c99 -fPIC -pedantic-errors \
	-Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition \
	-Wno-unused-result

# By default, keep a debug and release build available.
all: debug release

debug: $(OBJECTS_DEB)
	$(CC) $(LFLAGS) -o $(LIB_DEB) $(OBJECTS_DEB)

release: $(OBJECTS_REL)
	$(CC) $(LFLAGS) -o $(LIB_REL) $(OBJECTS_REL)

-include $(DEPS_DEB)

$(OBJECTS_DEB): $(patsubst ../build/debug/%.o,src/%.c,$@)
	$(CC) $(DEB_FLAGS) $(CFLAGS) $(patsubst ../build/debug/%.o,%.c,$@) -o $@

-include $(DEPS_REL)

$(OBJECTS_REL): $(patsubst build/release/%.o,src/%.c,$@)
	$(CC) $(REL_FLAGS) $(CFLAGS) $(patsubst ../build/release/%.o,%.c,$@) -o $@

# Clean up everything
clean:
	-rm $(LIB_DEB)
	-rm $(LIB_REL)
	-rm $(OBJECTS_DEB)
	-rm $(OBJECTS_REL)
	-rm $(DEPS_DEB)
	-rm $(DEPS_REL)
