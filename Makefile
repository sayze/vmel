# Project Files.
SRCDIR = src
INCDIR = $(SRCDIR)/include
SRCFILES = node.c tokenizer.c utils.c parser.c errors.c sytable.c nexec.c vmel.c 
LIBDIR = ./bin/debug/modules
LIBFILES = vstring
LIBS = $(addprefix $(LIBCDIR)/, $(LIBFILES))
SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
OBJS = $(SRCFILES:.c=.o)
OUT  = vmel

# Compiler generic flags.
CC = gcc
CFLAGS = -I$(INCDIR) -Wall -Werror -Wextra
LFLAGS = -L()

# Debug settings.
DBGDIR = bin/debug
DBGOUT = $(DBGDIR)/$(OUT)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

# Release settings.
RELDIR = bin/release
RELOUT = $(RELDIR)/$(OUT)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -DNDEBUG

# Debug Rules.
$(DBGOUT): $(DBGOBJS)
	$(CC) -L$(LIBDIR) -lvstring $(CFLAGS) $(DBGCFLAGS) -o $(DBGOUT) $^

$(DBGDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

# Release Rules.
$(RELOUT): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELOUT) $^

$(RELDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	mkdir -p $(DBGDIR) $(RELDIR)

remake: clean all

clean:
	rm -rf $(DBGOUT) $(DBGOBJS) $(RELOUT) $(RELOBJS)

# builds.
all: prep release	
debug: $(DBGOUT)
release: $(RELOUT)
