
# Note that building without FINAL=1 creates an uber-verbose version

ifneq ($(FINAL),1)
    VERSION:=\"0.5.1.interim.debug\"
else
    VERSION:=\"0.5.1\"
endif

CFLAGS=$$(pkg-config --cflags glib-2.0 libnautilus-extension)
LDFLAGS=$$(pkg-config --libs glib-2.0 libnautilus-extension)

CFLAGS+=-DPIC -fPIC -g -O -DVERSION=$(VERSION)
LDFLAGS+=-Wl,--as-needed -g -O

ifneq ($(FINAL),1)
    CFLAGS+=-D_DEBUG
endif

TARGET=libnautilus-follow-symlink

# REQUIRED TO BUILD!
TARGET_DIR=/usr/lib/nautilus/extensions-1.0
INSTALL_DIR=$(DESTDIR)$(TARGET_DIR)

CC:=gcc

all: $(TARGET).so

$(TARGET).la: follow-symlink.o nautilus-ext-follow-symlink.o
	libtool --mode=link $(CC) $(LDFLAGS) -o $(TARGET).la follow-symlink.lo nautilus-ext-follow-symlink.lo -rpath $(TARGET_DIR)

$(TARGET).so: $(TARGET).la
	ln -sf .libs/$(TARGET).so .

follow-symlink.o: follow-symlink.c follow-symlink.h common.h
	libtool --mode=compile $(CC) $(CFLAGS) -c follow-symlink.c

nautilus-ext-follow-symlink.o: nautilus-ext-follow-symlink.c nautilus-ext-follow-symlink.h common.h
	libtool --mode=compile $(CC) $(CFLAGS) -c nautilus-ext-follow-symlink.c

install: strip
	mkdir -p $(INSTALL_DIR)
	install -m644 -oroot -groot $(TARGET).so $(INSTALL_DIR)/

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET).so
	rmdir -p $(INSTALL_DIR) || true

strip: $(TARGET).so
	strip $(TARGET).so

distclean: clean

clean:
	rm -f *.la *.lo *.o *.so
	rm -rf .libs
