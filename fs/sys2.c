/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * main system calls are defined in this file
 */


#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * open a file in the memory:
 *
 * return a integer from 0 - 99 when succeed 
 *
 * return -1 when crash
 *
 */
int open(const char* pathname) {
	
	/* if exists */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no == 0) {
		printf("No such file or directory");
		return -1;
	}
	
	/* get inode */
	struct inode_t* pinode = iget(dinode_no);

	/* find free o_file in memory */
	int i;
	for (i=0; i<100; i++) {
		if (o_file[i].count == 0)
			break;
	}
	if (i == 100) {
		printf("Can't open more files");
		return -1;
	}
	o_file[i].pinode = pinode;
	o_file[i].count = 1; /* first open */
	return i;
}

void close(int no) {
	o_file[no].count = 0;
	return;
}

