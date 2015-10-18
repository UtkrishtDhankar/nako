CC=gcc
CFLAGS=-c -Wall -std=gnu99
LFLAGS=-Wall -std=gnu99

DFLAGS=-g -Werror

RM=rm
RMFLAGS=-f

# Source files in the project. Append new files here.
SRCS=init.c snap.c nako.c sha1.c switch.c

# Create object files list from source files list.
OBJS= $(SRCS:.c=.o)

all: nako
all: clean-objects

# clean completely wipes directory of all executables and objects
clean: clean-objects
	$(RM) $(RMFLAGS) nako

# only removes objects, not final executable
clean-objects:
	$(RM) $(RMFLAGS) *.o

debug: CFLAGS+=$(DFLAGS)
debug: LFLAGS+=$(DFLAGS)
debug: all

nako: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o nako

# the following magic code is from here:
# http://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
#
# Use with care. This automatically builds all .c files inside the folder.
.c.o:
	$(CC) $(CFLAGS) $<  -o $@
