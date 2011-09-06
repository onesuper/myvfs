/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * inode's allocation functions are defined in this file
 *
 */

#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * allocate an unused dinode and call
 * iget() to get a inode in the memory 
 * 
 * return a inode pointer if succeed
 * else return a NULL pointer
 */
struct inode_t *ialloc(void) {

	/* if running out of free inode */
	if (sb.free_inode_num == 0) {
		printf("running out of inode");
		return NULL;
	}

	/*
	 * if the free node stack is empty,
	 * search the inode area and find 50 free inodes
	 */
	unsigned long addr;
	int i;
	int count = 0;
	char find50 = 0;			/* set only if find 50 inodes */
	struct d_inode_t dinode;	
	if (sb.free_inode_sp == 0) {
		/* 
		 * search the entire inode area until find 50 free inodes
		 */
		for (i=0; i<SBLOCK * NIBLOCK / sizeof(dinode); i++) {
			addr = map_addr(i);		/* physical addr of d_inode*/
			fseek(fd, addr, 0);
			fread(&dinode, 1, sizeof(dinode), fd);
			/* only empty one can be alloced */
			if (dinode.type == 'e') {
				/* stack grows */
				sb.free_inode_stack[sb.free_inode_sp] = dinode.no;
				sb.free_inode_sp++;
				/*
				 * now change the d_inode's type and write back
				 */
				dinode.type = 's';	/* ??? */
				fseek(fd, addr, 0);
				fwrite(&dinode, 1, sizeof(dinode), fd);
				count ++;
				if (count == 50) {
					find50 = 1;
					break; 
				}
			}
		}
		/* can't find 50 free inodes */
		if (find50 = 0) { 
			printf("running out of on-disk inodes");
			return NULL;
		}
	}

	/* allocate the stack-top d_inode and save the super block */
	unsigned int dinode_no = sb.free_inode_stack[sb.free_inode_sp];
	sb.free_inode_sp--;
	sb.free_inode_num--;
	save_super_block();
	
	/* 
	 * call iget() to get the inode's pointer through dinode_no
	 * then return it
	 */
	struct inode_t* pinode = iget(dinode_no);
	return pinode;
}


/*
 * free a inode from disk
 */
void ifree(unsigned int dino_id) {

	/* 
	 * check the stack before push in case
	 * it will burst, if the stack is full,
	 * save a half to the disk
	 */
	if (sb.free_inode_sp == 100) {
		int i;
		unsigned int id;
		unsigned int addr;
		struct d_inode_t dino;
		for (i=0; i<50; i++) {
			id = sb.free_inode_stack[sb.free_inode_sp - 1];
			addr = map_addr(id);
			fseek(fd, addr, 0);
			fread(&dino, 1, sizeof(dino), fd);
			dino.type = 's';  /* ?? */
			fwrite(&dino, 1, sizeof(dino), fd);
		}
	}
	
	/* push the released d_inode to the free_inode_stack */
	sb.free_inode_stack[sb.free_inode_sp] = dino_id;
	sb.free_inode_sp++;
	sb.free_inode_num++;
	return;
}
