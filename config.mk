# xrectdraw version
VERSION = 0.0.1

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes and libs
INCS = -I$(X11INC)
LIBS = -L$(X11LIB) -lX11

# flags
CFLAGS = -Wall -pedantic -std=c99
CPPFLAGS = -DVERSION=\"$(VERSION)\" -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
STCFLAGS = $(INCS) $(CPPFLAGS) $(CFLAGS)
STLDFLAGS = $(LIBS) $(LDFLAGS)

# compiler and linker
CC = gcc
