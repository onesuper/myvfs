/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * block's allocation functions are defined in this file
 *
 */

#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *
 *
 */
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
				if (bmap.use[j] == 0) { /* if free */
					bmap.use[j] = 1;
					/* push into the stack */ 
					sb.free_block_stack[sb.free_block_sp] = bmap.addr[j];
					sb.free_block_sp--;
					count++;
					if (count == 50) {
						flag = 1;
						break;
					}
				}
			}
			/* change the 12 bmaps */
			fseek(fd, 2 + SBLOCK * 50 * i, 0);
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


/*
 *
 *
 *
 */
void bfree(unsigned int block_no) {
	
	/* 
	 * if the stack is full, pop out 50 elements
	 * at one time
	 */
	struct bmap_t bmap;
	int offset, i;
	unsigned int block_no_temp;
	if (sb.free_block_sp == 100) {
		for(i=0; i<50; i++) {
			block_no_temp = sb.free_block_stack[sb.free_block_sp - 1];
			sb.free_block_sp--; /* pop a block */

			/* in order to find the bit
			 * in the bmap to set
			 */
			offset = (block_no_temp - 2) % 50;

			fseek(fd, SBLOCK * block_no_temp, 0);
			fread(&bmap, 1, sizeof(bmap), fd);
			bmap.free_block_num++;
			bmap.use[offset-1] = 0; /* not in use now */
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


