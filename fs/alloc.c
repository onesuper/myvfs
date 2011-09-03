/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * inode's and block's allocation functions
 * are defined in this file
 *
 */

#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * alloc a inode on the disk
 */
struct inode_t *ialloc(void) {

	/* no free inode left */
	if (sb.free_inode_num == 0) {
		printf("running out of inode");
		return NULL;
	}
	

	/* put more free inode in the stack */
	if (sb.free_inode_sp == 0) {

		struct d_inode_t dino; /* the readin on-disk inode */
		/* 
		 * search the on-disk inode area
		 * and find 50 empty inode to supply the stack
		 */
		

		/* destination */
		unsigned long addr;
		unsigned int i;
		int count = 0;

		for (i=0; i<NIBLOCK * SBLOCK / sizeof(dino); i++) {
			addr = map_addr(i);  /* find the physical block */
			fseek(fd, addr, 0);
			fread(&dino, 1, sizeof(dino), fd);
			if (dino.type == 'e') {
				/* stack grows */
				sb.free_inode_stack[sb.free_inode_sp] = dino.no;
				sb.free_inode_sp ++;
				
				/* change the state of the dino and write back */
				dino.type = 's';
				fseek(fd, addr, 0);
				fwrite(&dino, 1, sizeof(dino), fd);
				count ++;
				if (count == 50) {
					break; 
				}
			}
		}
	}

	/* allocate the on-disk inode in the top of stack */
	unsigned int no_to_alloc = sb.free_inode_stack[sb.free_inode_sp];
	
	/* find the inode to alloc by iget() */
	struct inode_t *temp_inode = iget(no_to_alloc);
	
	/* */
	unsigned long alloc_addr = map_addr(no_to_alloc);
	fseek(fd, alloc_addr, 0);
	fwrite(&temp_inode->dnum, 1, sizeof(temp_inode), fd);
	sb.free_inode_sp--;
	sb.free_inode_num--;

	save_super_block();

	return temp_inode;

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


unsigned int balloc(void) {

	if (sb.free_block_num == 0) {
		printf("running out of free blocks");
		return 0;
	}

	/* 
	 * if the stack is running out of free blocks,
	 * supply 50 elements into it
	 */
	if (sb.free_block_sp == 0) {
		int i, j;
		int count = 0;
		char flag = 0; /* set when the stack has 50 elements */
		/* #402 to #1002, 12 * 50 = 600 */
		for (i=0; i < 12; i++) {
			struct bmap_t bmap;
			/* read the bmap from disk */
			fseek(fd, 2 + SBLOCK * 50 *i, 0);
			fread(&bmap, 1, sizeof(bmap), fd);
			for (j=0; j<49; j++) {
				if (map.use[j] == 0) { /* if free */
					map.use[j] = 1;
					/* push into the stack */ 
					sb.free_block_stack[sb.free_block_sp] = map.addr[j];
					sb.free_block_sp--;
					count++;
					if (count == 50) {
						flag = 1;
						break;
					}
				}
			}
			/* change the 12 bmaps */
			fseek(fdm, 2 + SBLOCK * 50 * i, 0);
			fwrite(&bmap, 1, sizeof(bmap), fd);
			if (flag == 1)
				break;
		}
	}

	/*  now alloc the stack-top free block */
	unsigned int free_block_no = sb.free_block_stack[sb.free_block_sp];
	sb.modified = 0;  
	sb.free_block_num--;
	sb.free_block_sp--;
	save_super_block();
	return free_block_no;	
}

void bfree(unsigned int block_no) {
	
	
	/* 
	 * if the stack is full, pop out 50 elements
	 * at one time
	 */
	struct bmap_t bmap;
	int offset;
	unsigned int block_no;
	if (sb.free_block_sp == 100) {
		for(i=0; i<50; i++) {
			block_no = sb.free_block_stack[sb.free_block_sp - 1];
			sb.free_block--; /* pop a block */

			/* in order to find the bit
			 * in the bmap to set
			 */
			offset = (block_no - 2) % 50;

			fseek(fd, SBLOCK * block_no, 0);
			fread(&bmap, 1, sizeof(bmap), fd);
			map.free_block_num++;
			map.use[offset-1] = 0; /* not in use now */
			fwrite(&bmap, 1, sizeof(bmap), fd); /* write back */
		}
	}
	
	/* push the new free block on to stack */
	sb.free_block_stack[sb.free_block_sp] = block_no;
	sb.free_block_sp++;
	sb.free_block_num++;
	sb.modified = 0;
	save_super_block();
	return;
}


