# Project Files.
SRCDIR = src
SRCFILES = vmel.c
SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
OBJS = $(SRCFILES:.c=.o)
OUT  = vmel

# Compiler generic flags.
CC = gcc
CFLAGS = -I$(SRCDIR) -Wall -Werror -Wextra

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
debug: $(DBGOUT)

$(DBGOUT): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGOUT) $^

$(DBGDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

# Release Rules.
release: $(RELOUT)

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