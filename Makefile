CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra
CFLAGS += -pthread
PKG_CONFIG ?= pkg-config

RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib)
RAYLIB_LIBS := $(shell $(PKG_CONFIG) --libs raylib)
LDLIBS ?= -lm
LDLIBS += -pthread

SOURCES := src/*.c 
HEADERS := src/*.h 
TARGET := cractal 

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(RAYLIB_CFLAGS) $(SOURCES) -o $@ $(LDFLAGS) $(RAYLIB_LIBS) $(LDLIBS)

clean:
	rm -f $(TARGET)
