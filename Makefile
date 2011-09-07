# ========================================================
#     Global
# ========================================================
VERSION = 0.1.0
AUTHOR = onesuper
CC = gcc
CFLAGS = -c -fPIC -I. -Wall
DEBUG = -g	
OBJECTS = fs/util.o fs/mount.o fs/ialloc.o fs/balloc.o fs/iget.o fs/namei.o fs/sys.o fs/sys2.o 
TFLAGS = -c -I. -Wall
TOBJ = test/test_util.o


# ========================================================
#            ALL
# ========================================================
all: init/init.so fs/fs.so

test: test/test_ls test/test_init test/test_mkdir


# =========================================================
#     unit test
# =========================================================

# test_init 
test/test_init: test/test_init.o init/init.o
	$(CC) -o test/test_init test/test_init.o init/init.o $(DEBUG)

test/test_init.o: test/test_init.c
	$(CC) -o test/test_init.o $(TFLAGS) test/test_init.c $(DEBUG)


# test_ls 
test/test_ls: test/test_ls.o $(TOBJ)
	$(CC) -o test/test_ls test/test_ls.o $(OBJECTS) $(TOBJ) $(DEBUG)

test/test_ls.o: test/test_ls.c
	$(CC) -o test/test_ls.o $(TFLAGS) test/test_ls.c $(DEBUG)


# test_mkdir
test/test_mkdir: test/test_mkdir.o $(TOBJ)
	$(CC) -o test/test_mkdir test/test_mkdir.o $(OBJECTS) $(TOBJ) $(DEBUG)

test/test_mkdir.o: test/test_mkdir.c
	$(CC) -o test/test_mkdir.o $(TFLAGS) test/test_mkdir.c $(DEBUG)

# test_util 
$(TOBJ): test/test_util.c
	$(CC) -o $(TOBJ) $(TFLAGS) test/test_util.c $(DEBUG)



# ======================================================
#    init.so
# ======================================================
init/init.so: init/init.o
	$(CC) -o init/init.so -shared init/init.o $(DEBUG)

init/init.o: init/init.c
	$(CC) -o init/init.o $(CFLAGS) init/init.c $(DEBUG)


# =====================================================
#     fs.so
# =====================================================
fs/fs.so: $(OBJECTS)
	$(CC) -o fs/fs.so -shared $(OBJECTS) $(DEBUG)


# ======================================================
#     OBJECTS
# ======================================================
fs/mount.o: fs/mount.c
	$(CC) -o fs/mount.o $(CFLAGS) fs/mount.c $(DEBUG)

fs/utils.o: fs/util.c
	$(CC) -o fs/util.o $(CFLAGS) fs/util.c $(DEBUG)

fs/ialloc.o: fs/ialloc.c
	$(CC) -o fs/ialloc.o $(CFLAGS) fs/ialloc.c $(DEBUG)

fs/balloc.o: fs/balloc.c
	$(CC) -o fs/balloc.o $(CFLAGS) fs/balloc.c $(DEBUG)

fs/iget.o: fs/iget.c
	$(CC) -o fs/iget.o $(CFLAGS) fs/iget.c $(DEBUG)

fs/namei.o: fs/namei.c
	$(CC) -o fs/namei.o $(CFLAGS) fs/namei.c $(DEBUG)

fs/sys.o: fs/sys.c
	$(CC) -o fs/sys.o $(CFLAGS) fs/sys.c $(DEBUG)

fs/sys2.o: fs/sys2.c
	$(CC) -o fs/sys2.o $(CFLAGS) fs/sys2.c $(DEBUG)

# ======================================================
#      CLEAN
# ======================================================
clean:
	rm -rf init/*.o
	rm -rf fs/*.o
	rm -rf test/*.o

