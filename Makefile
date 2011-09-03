VERSION = 0.1.0
AUTHOR = onesuper
CC = gcc


all: init/init.so fs/fs.so

# init ----------------------------------------------------- PART I

init/init.so: init/init.o
	$(CC) -o init/init.so -shared init/init.o 

init/init.o: init/init.c
	$(CC) -o init/init.o -c -fPIC init/init.c -I. -Wall

# system calls in vfs ----------------------------------------- PART II

CFLAGS = -c -fPIC -I. -Wall

# add the objects here:

OBJECTS = fs/util.o fs/mount.o fs/alloc.o fs/iget.o fs/sys.o fs/namei.o

# gather all the object file and generate a big
# shared library

fs/fs.so: $(OBJECTS)
	$(CC) -o fs/fs.so -shared $(OBJECTS)


# Objects needed by target
fs/mount.o: fs/mount.c
	$(CC) -o fs/mount.o  $(CFLAGS) fs/mount.c

fs/utils.o: fs/util.c
	$(CC) -o fs/util.o  $(CFLAGS) fs/util.c 

fs/alloc.o: fs/alloc.c
	$(CC) -o fs/alloc.o $(CFLAGS) fs/alloc.c

fs/iget.o: fs/iget.c
	$(CC) -o fs/iget.o $(CFLAGS) fs/iget.c

fs/sys.o: fs/sys.c
	$(CC) -o fs/sys.o $(CFLAGS) fs/sys.c

fs/namei.o: fs/namei.c
	$(CC) -o fs/namei.o $(CFLAGS) fs/namei.c

clean:
	rm -rf init/*.o
	rm -rf fs/*.o
