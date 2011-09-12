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
int mopen(const char* pathname) {
	
	/* if exists */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no == 0) {
		printf("No such file or directory\n");
		return -1;
	}
	
	/* get inode */
	struct inode_t* pinode = iget(dinode_no);
	
	/* ensure to open a file */
	if (pinode->type != 'f') {
		printf("Is a directory\n");
		iput(pinode);
		return -1;
	}

	/* find a open_file which is not at work */
	int i;
	for (i=0; i<NOFILE; i++) {
		if (open_file[i].count == 0)
			break;
	}
	if (i == NOFILE) {
		printf("Cannot open any more files\n");
		return -1;
	}
	open_file[i].pinode = pinode;
	open_file[i].count = 1;			/* open for first time */
	return i;
}

/*
 * close the open file
 */
void mclose(int file_no) {
	open_file[file_no].count = 0;
	/* iput the inode */
	iput(open_file[file_no].pinode);
	return;
}

/*
 * read the open file
 */
char* mread(int file_no) {

	/* check the correctness of file_no */
	if (file_no < 0 || file_no > 99) {
		printf("File has not been opened\n");
		return "";
	}

	/* ensure open */
	if (open_file[file_no].count < 1) {
		printf("File has not been opened\n");
		return "";
	}

	/* find the inode pointer in open_file[] */
	struct inode_t* pinode = open_file[file_no].pinode;
	pinode = iget(pinode->dino);
	if (pinode->size < 1) {
		iput(pinode);
		return "";
	} else {
		/* the reading area is decided by pinode->size */
		char *buf = (char*)malloc(pinode->size * SBLOCK);
		int i;
		/* read block by block */
		for (i=0; i<pinode->size; i++) {
			fseek(fd, pinode->addr[i] * SBLOCK, 0);
			fread(buf + i * SBLOCK, 1, SBLOCK, fd);
		}
		iput(pinode);
		return buf;
	}
}

/* 
 * write to a open file
 */
void mwrite(int file_no, const char* content) {

	/* check the correctness of file_no */
	if (file_no < 0 || file_no > 99) {
		printf("File has not been opened\n");
		return;
	}

	/* ensure open */
	if (open_file[file_no].count < 1) {
		printf("File has not been opened\n");
		return;
	}

	/* find the inode pointer in open_file[] */
	struct inode_t* pinode = open_file[file_no].pinode;
	pinode = iget(pinode->dino);

	/* calculate the length of the content */
	int length;
	for (length=0; *(content + length) != '\0'; length++)
		;
	/* 1023=>1, 1024=>1, 1025=>2 */
	unsigned int block_num = length * sizeof(char) / SBLOCK;
	unsigned int offset = length * sizeof(char) % SBLOCK;
	if (offset != 0) 
		block_num++;
	/*
	 * before allocation, release the blocks that 
	 * the file has taken
	 */
	int i;
	for (i=0; i<pinode->size; i++) {
		bfree(pinode->addr[i]);
	}

	/* allocate the blocks for the file */
	for (i=0; i < block_num; i++) {
		unsigned int block_no = balloc();
		if (block_no == 0) 
			return;
		pinode->addr[i]= block_no;
		pinode->size++;
		/*
		 * write a block-size of content to the block
		 */
		fseek(fd, block_no * SBLOCK, 0);
		fwrite(content + i * SBLOCK, 1, SBLOCK, fd);
	}
	iput(pinode);
	return;
}
