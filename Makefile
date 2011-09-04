# Global
VERSION = 0.1.0
AUTHOR = onesuper
CC = gcc
CFLAGS = -c -fPIC -I. -Wall



# All Targets
all: init/init.so fs/fs.so



#	PART I   
init/init.so: init/init.o
	$(CC) -o init/init.so -shared init/init.o 

init/init.o: init/init.c
	$(CC) -o init/init.o $(CFLAGS) init/init.c

#	PART II
# add the objects here:
OBJECTS = fs/util.o fs/mount.o fs/ialloc.o fs/balloc.o fs/iget.o fs/namei.o fs/sys.o fs/sys2.o 

# gather all the objects into a big shared library
fs/fs.so: $(OBJECTS)
	$(CC) -o fs/fs.so -shared $(OBJECTS)


# Objects needed by target
fs/mount.o: fs/mount.c
	$(CC) -o fs/mount.o  $(CFLAGS) fs/mount.c

fs/utils.o: fs/util.c
	$(CC) -o fs/util.o  $(CFLAGS) fs/util.c 

fs/ialloc.o: fs/ialloc.c
	$(CC) -o fs/ialloc.o $(CFLAGS) fs/ialloc.c

fs/balloc.o: fs/balloc.c
	$(CC) -o fs/balloc.o $(CFLAGS) fs/balloc.c

fs/iget.o: fs/iget.c
	$(CC) -o fs/iget.o $(CFLAGS) fs/iget.c

fs/namei.o: fs/namei.c
	$(CC) -o fs/namei.o $(CFLAGS) fs/namei.c

fs/sys.o: fs/sys.c
	$(CC) -o fs/sys.o $(CFLAGS) fs/sys.c

fs/sys2.o: fs/sys2.c
	$(CC) -o fs/sys2.o $(CFLAGS) fs/sys2.c

# clean
clean:
	rm -rf init/*.o
	rm -rf fs/*.o
