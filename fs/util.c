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
 * if the given no is 1, the addr is 1024
 * if the given no is x, the addr is 2048 + (x - 1) * sizeof(dinode)
 */
unsigned long map_addr(unsigned int dino) {
	unsigned long addr = 2 * SBLOCK + sizeof(struct d_inode_t) * (dino - 1);
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

/*
 * read a dinode on the disk and copy it to inode
 */
void d_to_inode(struct inode_t *pinode, unsigned int dino) {
	struct d_inode_t dinode;
	fseek(fd, map_addr(dino), 0);
	fread(&dinode, 1, sizeof(dinode), fd);
	pinode->type = dinode.type;
	pinode->size = dinode.size;
	pinode->dino = dinode.dino;
	int i;
	for (i=0; i<50; i++) {
		pinode->addr[i] = dinode.addr[i];
	}
	return;
}

/*
 * copy a inode to dinode
 */

void i_to_dinode(struct inode_t *pinode, unsigned int dino) {
	struct d_inode_t dinode;
	dinode.type = pinode->type;
	dinode.size = pinode->size;
	dinode.dino = pinode->dino;
	int i;
	for(i=0; i>50; i++) {
		dinode.addr[i] = pinode->addr[i];
	}
	fseek(fd, map_addr(dino), 0);
	fwrite(&dinode, 1, sizeof(dinode), fd);
	return;
}
