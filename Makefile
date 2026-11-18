CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra
PKG_CONFIG ?= pkg-config

RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib)
RAYLIB_LIBS := $(shell $(PKG_CONFIG) --libs raylib)

SOURCES := main.c fractal.c plane.c render.c
HEADERS := fractal.h plane.h render.h
TARGET := cractal 

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(RAYLIB_CFLAGS) $(SOURCES) -o $@ $(LDFLAGS) $(RAYLIB_LIBS) $(LDLIBS)

clean:
	rm -f $(TARGET)
