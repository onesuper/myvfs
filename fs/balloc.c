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
 * allocate a block from the free block stack
 *
 * return a block no from if succeed
 * else return 0
 */
unsigned int balloc(void) {

	/* if there is any free block */
	if (sb.free_block_num == 0) {
		printf("running out of free blocks");
		return 0;
	}

	/* 
	 * before allocating a block from the free block stack
	 * check if the stack is running out of free blocks
	 * if the stack is empty, supply 50 elements
	 *
	 * to find 50 free block, the balloc() will search all
	 * the bmaps at #402...#1002
	 */
	if (sb.free_block_sp == 0) {
		int i, j;
		int count = 0;
		char flag = 0;		/* set when 50 elements came in*/

		/* [402, 1002)  600 / 50 = 12 */
		for (i=0; i<NDBLOCK / 50 ; i++) {
			struct bmap_t bmap;
			/* read the bmap from disk */
			fseek(fd, SBLOCK * (2 + NIBLOCK + 50 * i), 0);
			fread(&bmap, 1, sizeof(bmap), fd);
			/* search use[1]...use[49] */
			for (j=1; j<50; j++) {
				if (bmap.use[j] == 0) { /* if free */
					bmap.use[j] = 1;
					bmap.free_block_num --;
					/* push into the stack */ 
					sb.free_block_stack[sb.free_block_sp] = bmap.addr[j];
					sb.free_block_sp ++;
					sb.free_block_num --;
					count++;
					/* 50 elements is enough */
					if (count == 50) {
						flag = 1;
						break;
					}
				}
			}
			/* alter the bitmaps */
			fseek(fd, SBLOCK * (2 + NIBLOCK + 50 * i), 0);
			fwrite(&bmap, 1, sizeof(bmap), fd);
			if (flag == 1)
				break;
		}
	}

	/* now allocate the stack-top free block */
	unsigned int alloc_block_no = sb.free_block_stack[sb.free_block_sp - 1];
	sb.modified = 0;
	sb.free_block_num--;
	sb.free_block_sp--;
	save_super_block();	/* save the super_block */

	return alloc_block_no;	
}


/*
 * recycle a using block to the free block stack
 *
 * accept a block_no as argument 
 */
void bfree(unsigned int block_no) {
	
	/* 
	 * before putting the free_block back to the stack
	 * bfree() will check if the stack is already full
	 * if the stack is full, pop out 50 elements at one
	 * time BECAUSE it takes too much time to look up
	 * things in bitmaps frequently.
	 */
	if (sb.free_block_sp == 100) {
		/* dump 50 elements */
		struct bmap_t bmap;
		unsigned int block_no_temp;
		int i, offset;
		/*
		 * 50 loops
		 */
		for(i=0; i<50; i++) {
			block_no_temp = sb.free_block_stack[sb.free_block_sp - 1];
			sb.free_block_sp--; /* pop a block */
			/* 
			 * find the exact bit's offet in the bitmap to clear
			 * if the block_no is #403, it the use[1] should be clear 
			 * offset = (403 - 2) % 50 = 1
			 */
			offset = (block_no_temp - 2) % 50;
			fseek(fd, SBLOCK * block_no_temp, 0);
			fread(&bmap, 1, sizeof(bmap), fd);
			bmap.free_block_num++;
			bmap.use[offset] = 0;					/* not in use now */
			fwrite(&bmap, 1, sizeof(bmap), fd);		/* write back */
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


