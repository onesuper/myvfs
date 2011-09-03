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



/* 
 * list the all the files and child directories
 * under current directory
 *
 * require:
 *	iget.c: iget()
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
	for (i=0; i<dir.size; i++) {
		printf("%s\t", dir.files[i].name);
	}
	return;
}


/*
 * change to a directory
 *
 */
void cd(const char* pathname) {
	int dino_no = namei(pathname);

	if (dino_no == 0) {
		printf("No such file or directory");
		return;
	}
	
	struct inode_t* pinode = iget(dino_no);

	if (pinode == NULL) {
		printf("No such file or directory");
		return;
	}

	if (pinode->type != 'd') {
		printf("Not a directory");
		return;
	}

	cur_dir_inode_no = dino_no;
	
}

/*
 * create a directory under the current directory.
 *
 * NOTE: THIS FUNCTION DID NOT CHECK THE CORRECTNESS
 * OF PATHNAME, so make sure you path a correct
 * pathname to it.
 *
 */
void mkdir(const char* pathname) {

	/* search it first */
	unsigned int dino_id = namei(pathname);
	/* if it already exists, give up */
	if (dino_id != 0) {
		printf("File or directory exists");
		return;
	}





	return;
}


/*
 * create a file under the current directory.
 *
 * NOTE: THIS FUNCTION DID NOT CHECK THE CORRECTNESS
 * OF PATHNAME, so make sure you path a correct
 * pathname to it.
 *
 */

void touch(const char* pathname) {
	return;
}


void rm(const char* pathname) {
	return;
}

void rmdir(const char* pathname) {
	return;
}
