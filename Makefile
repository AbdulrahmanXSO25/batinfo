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
	install -v -Dm755 $(TARGET) $(BINDIR)/$(TARGET)
	install -v -Dm644 $(MANPAGE) $(MANDIR)/$(MANPAGE)
	gzip -fv $(MANDIR)/$(MANPAGE)

# Clean up the build
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all install clean