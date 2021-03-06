

## What It Is

Myvfs is a virtual file system to show how a file system
is implemented. In other word, how the OS manage its disk.


Myvfs can be considered as a child of UNIX's file system.
Because it uses most concepts of UNIX, such as 'inode',
which originally came from the UNIX's world.


But it is a simple one. The only knowledge required to 
write it is probably C language. So one can focus on the
implementation of underlying data structures in most of
the time and never bother to care about the hardware bec-
ause it is not a real file system at all.


Myvfs uses a binary file to play as a device and commun-
icate with the real file system with fwrite() and fread()
which are two library functions of C.


There are two layers: core and shell. Main data structures 
and procedures are defined in the core layer. This part is 
written in C.


Shell/utility layer calls the APIs provided by the core layer
and deal with the interactive stuffs, which is written in
Python.


The system looks like:

<pre>
         
Python        shell/utility           
         =====================	   -+
           cd ls mkdir touch        |
 API        rmdir open rm           |
            open write read         |   virtual file sysetm     
         ---------------------      |        
  C               core              |
         =====================     -+
c lib        fread  fwrite      
         ---------------------
            real file system
         ---------------------
              real device
</pre>


With Python one can add other features easily and make the
shell look great. For example, if you want to make your file
system a C-S model, no more than twenty lines of socket is
needed. 


To make the program easy to debug, myvfs provides a test
framework to test procedure in a more clear way. (See 'Use
of the Test Framework')


## Is Not

Myvfs is not a real file system which is a synonym for
quickness, efficiency and reliability. On the contrary, it
is more like a cookbook to read or a toy to play with. So
the conciseness and readability are more important factors.


I have heard a thousand of times that UNIX's file system
is already simple. However, I think it is simple to a mo-
dern operating system, but not simple enough to a textbook
for students to learn. I have two reasons:


Like the way people learn the compiler construction. First
they take several theory courses on linguistics and optim-
ization. Then they go home and implement their own one. 


So a syntax called PL/0 is popular in courses through out
the world. Because it is rather easy for students to write
a recursive descent parser for such a simple syntax.


This is the only way they will not lose their interest in
the early stage and work out the prototype as soon as
possible. 


As long as the 0.1 version is done, the 2.0 version is vis-
ible. Student can improve it with advanced features like
parameter passing to procedures or data structures like
arrays, strings or floating point numbers.


The same thought can be seen in software development. So
'agile' seems to be the hottest word in recent years.


Premature optimization is harmful, but we could see them a
lot in UNIX's source code. Many procedures have been written
very early and thoroughly debugged and then left unchanged.


The most famous one maybe "namei", which John Lions quipped
as "it would not win the prize for 'Procedure of the Year'".


The second reason is UNIX is wrote for PDP11. The C language
is always highly related to the machine. Some parts are even
written in assembly language which can never be seen today.
If we can make the file system more abstract, things will be
easier.


## Getting Started

1. Compile the source code:			
  
    $make

2. Install the vfs on your disk:
  
    $python install.py

3. Run the shell to access to the vfs:

    $python shell.py
    myvfs 0.1
    Type "help" or "man" for more information.
    >>> 

4. Type 'help' to get help:
  
    >>> help


## Description of Contents
	
<pre>

  |-- fs
  |   |-- balloc.c
  |   |-- ialloc.c
  |   |-- iget.c
  |   |-- ...
  |   `-- fs.so
  |
  |-- include
  |   |-- fs.h
  |   |-- param.h
  |   `-- test.h
  |
  |-- init
  |   |-- init.c
  |   `-- init.so
  |
  |-- test
  |   |-- test_util.c
  |   |-- test_ls.c
  |   |-- test_cd.c
  |   |-- test_mkdir.c
  |   |-- ...
  |   `-- volume2.vfs
  |
  |-- install.py
  |
  |-- Makefile
  |
  |-- myvfs.py
  |
  `-- volume1.vfs

</pre>

### fs


The core part of the file system. All the source code of
system calls and procedures are in this folder. They are
linked into a dynamic link library (fs.so) which can be
directly called in myvfs.py.

### include

C header files of the file system (fs.h), testing framework
(test.h) and the constants (param.h).

### init

This file is involved with the initialization when the file
system is fisrt time installed to the computer. Also it
provides a shared library (init.so) to call it from outside.

### test

The testing framework file. The file test\_utils.c defines
some useful procedures which can be used to watch the state
of vfs when debugging. The other source file is for unit
test. For example, test\_mkdir.c tests the mkdir system call
of vfs. It is better than testing everything in one file.

### intall.py

A Python script for installing vfs

### Makefile

GNU Make 3.82 or above is needed

### myvfs.py

The shell of myvfs

### volume1.vfs

The virtual device

	

## Use of the Test Framework

Assume you want to test the procedure ls():


1. Compile the source code:

    $make 

2. Compile the test framework:

    $make test_ls

3. Change to the dir:

    $cd test

4. Use GDB to debug:

    $gdb test_ls
    (gdb)

5. Type any commands in gdb like:
  
    (gdb) b 16

6. Use the method provided by the framework:
  
    (gdb) watch_dinode()


### Test Methods

1. **picture**: show all the infomations of the vfs like picturing it
2. **watch_dinode**: watch a device node's infomation on the vfs
3. **watch_inode**: watch a inode in the memory
4. **watch_usr**: watch user infomation
