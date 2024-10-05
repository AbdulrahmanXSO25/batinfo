# Define variables
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = batinfo
SRCS = batinfo.c
BINDIR = /usr/local/bin
MANDIR = /usr/share/man/man1
MANPAGE = batinfo.1

# Default target
all: $(TARGET)

# Compile the source file into an executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Install the binary and the man page to the specified directories
install: clean $(TARGET) $(MANPAGE)
	sudo mv $(TARGET) $(BINDIR)
	sudo chmod +x $(BINDIR)/$(TARGET)
	sudo cp $(MANPAGE) $(MANDIR)
	sudo gzip -f $(MANDIR)/$(MANPAGE)  # Compress the man page

# Clean up the build
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all install clean

