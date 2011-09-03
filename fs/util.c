/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 *
 * some useful tools are defined here
 */



#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* save the superblock from memory to disk */
void save_super_block() {
	fseek(fd, 1, 0);
	fwrite(&sb, 1, sizeof(sb), fd);
	return;
}

/* find the position of d_inode according to the no */
unsigned long map_addr(unsigned int no) {
	struct d_inode_t dino;
	unsigned long addr = 2 + sizeof(dino) * (no - 1);
	return addr;
}

/* 
 * judge if the password is correct
 */
char verify(char* password) {
	if (strcmp(password, usr.password) == 0)
		return 1;
	else
		return 0;
}
