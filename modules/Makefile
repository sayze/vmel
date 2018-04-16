# Project Files.
SRCDIR = .
INCDIR = $(SRCDIR)/include

# Compiler generic flags.
CC = gcc
CFLAGS = -I$(INCDIR) -c -fPIC -Wall -Werror -Wextra
SFLAGS = -dynamiclib -o

# Platform specific
LIBEXT = dylib

# Debug settings.
DBGDIR = ../bin/debug/modules
DBGFLAGS = -g -O0 -DDBUG

# Release settings.
RELDIR = ../bin/release/modules
RELCFLAGS = -O3 -DNDEBUG

#list of libraries
LIBVSTRING = libvstring

# library builds.
vstring: vstring.o
	$(CC) $(SFLAGS) $(DBGDIR)/vstring.o -install_name $(DBGDIR)/$(LIBVSTRING).$(LIBEXT) $(DBGDIR)/$<

vstring.o: vstring.c
	$(CC) $(CFLAGS) $(DBGFLAGS) -o $(DBGDIR)/$@ $(SRCDIR)/$<

#
# Other rules
#
prep:
	mkdir -p $(DBGDIR) $(RELDIR)

remake: clean all

clean:
	rm -rf $(DBGDIR)/* $(RELDIR)/*


