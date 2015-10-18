CC=gcc
CFLAGS=-c -Wall -std=gnu99
LFLAGS=-Wall -std=gnu99

DFLAGS=-g -Werror

RM=rm
RMFLAGS=-f

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

nako: init.o snap.o nako.o sha1.o
	$(CC) $(LFLAGS) init.o snap.o sha1.o nako.o -o nako

init.o:
	$(CC) $(CFLAGS) init.c -o init.o

snap.o:
	$(CC) $(CFLAGS) snap.c -o snap.o

sha1.o:
	$(CC) $(CFLAGS) sha1.c -o sha1.o

nako.o:
	$(CC) $(CFLAGS) nako.c -o nako.o
