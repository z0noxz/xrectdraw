.POSIX:

include config.mk

SRC = xrectdraw.c
OBJ = $(SRC:.c=.o)

all: options xrectdraw

options:
	@echo xrectdraw build options:
	@echo "VERSION = $(VERSION)"
	@echo "CFLAGS  = $(STCFLAGS)"
	@echo "LDFLAGS = $(STLDFLAGS)"
	@echo "CC      = $(CC)"

.c.o:
	@echo CC $<
	@$(CC) $(STCFLAGS) -c $<

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

xrectdraw: $(OBJ)
	@echo CC -o $@
	@$(CC) -o $@ $(OBJ) $(STLDFLAGS)

clean:
	@echo cleaning
	@rm -f xrectdraw $(OBJ)

install: xrectdraw
	@echo installing executables to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	@cp -f xrectdraw ${PREFIX}/bin
	@chmod 755 ${PREFIX}/bin/xrectdraw
#	@echo installing manual page to ${MANPREFIX}/man1
#	@mkdir -p ${MANPREFIX}/man1
#	@cp -f xrectdraw.1 ${MANPREFIX}/man1
#	@chmod 644 ${MANPREFIX}/man1/xrectdraw.1

uninstall:
	@echo removing executable files from ${PREFIX}/bin
	@rm -f ${PREFIX}/bin/xrectdraw
#	@echo removing manual page from ${MANPREFIX}/man1
#	@rm -f ${MANPREFIX}/man1/xrectdraw.1

.PHONY: all options clean install uninstall
