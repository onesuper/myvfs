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


/* save sb from memory to disk */
void save_super_block() {
	fseek(fd, SBLOCK * 1, 0);
	fwrite(&sb, 1, sizeof(sb), fd);
	return;
}

/*
 * find dinode's physical address according to no
 * since the dinode are alloced sequently
 * if the given no is 1, the addr is 2
 * if the given no is x, the addr is 2 + (x - 1) * sizeof(dinode)
 */
unsigned long map_addr(unsigned int no) {
	struct d_inode_t dinode;
	unsigned long addr = 2 * SBLOCK + sizeof(dinode) * (no - 1);
	return addr;
}

/* 
 * verify the password
 */
char verify(char* password) {
	if (strcmp(password, usr.password) == 0)
		return 1;
	else
		return 0;
}
