CC=gcc
CFLAGS=-Wall -std=gnu99

RM=rm
RMFLAGS=-f

all: nako
all: clean-objects

# clean completely wipes directory of all executables and objects
clean: clean-objects
	$(RM) $(RMFLAGS) nako

# only removes objects, not final executable
clean-objects:
	$(RM) $(RMFLAGS) init.o snap.o nako.o sha1.o

debug: CFLAGS+=-g
debug: all

nako: init.o snap.o nako.o
	$(CC) $(CFLAGS) init.o snap.o sha1.o nako.o -o nako

init.o:
	$(CC) $(CFLAGS) -c init.c -o init.o

snap.o: sha1.o
	$(CC) $(CFLAGS) -c snap.c -o snap.o

sha1.o:
	$(CC) $(CFLAGS) -c sha1.c -o sha1.o

nako.o:
	$(CC) $(CFLAGS) -c nako.c -o nako.o
