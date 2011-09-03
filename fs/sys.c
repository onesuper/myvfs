#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>

/* 
 * list the all the files and child dorectories
 * under current directory
 *
 * dependencies:
 * require:
 * iget.c: iget()
 */

void ls(void) {
	
	/*
	 * in order to see what is in the directory entries
	 * get it via iget(), and then drop it via iput() 
	 */
	struct directory_t dir;
	struct inode_t* pinode= iget(cur_dir_inode_no);
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd);
	iput(pinode);

	int i;
	for (i=0; i<;) {
	}


	return;
}
