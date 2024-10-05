# Define variables
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = batinfo
SRCS = batinfo.c
BINDIR = /usr/bin
MANDIR = /usr/share/man/man1
MANPAGE = batinfo.1

# Default target
all: $(TARGET)

# Compile the source file into an executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Install the binary and the man page to the specified directories
install: $(TARGET) $(MANPAGE)
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	install -Dm644 $(MANPAGE) $(DESTDIR)$(MANDIR)/$(MANPAGE)
	gzip -f $(DESTDIR)$(MANDIR)/$(MANPAGE)  # Compress the man page
	$(MAKE) clean

# Clean up the build
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all install clean
