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
 * list the all the files and child directories
 * under current directory
 *
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

	/* search if it is ready there */
	unsigned int dino_no = namei(pathname);
	if (dino_no != 0) {
		printf("File or directory exists");
		return;
	}
	
	/* make a new directory entry */
	struct directory_t new_dir;

	/* 
	 * even a empty dir has a entry which
	 * represents its upper layer dir, in this case
	 * cur_dir
	 */
	new_dir.size = 1;
	strcpy(new_dir.files[0].name, "..");
	new_dir.files[0].dino = cur_dir_inode_no;

	/* alloc a  new block */
	unsigned int block_no = balloc();  
	if (block_no == 0)
		return;	

	/* alloc an new inode */
	struct inode_t pinode = ialloc();
	if (pinode == NULL) {
		bfree(block_no);  /* rollback~ */
		return;
	}

	/* write the directory to the new block */
	fseek(fd, block_no * SBLOCK, 0);
	fwrite(&new_dir, 1, sizeof(new_dir), fd);

	/* register inode information */
	pinode->size = 1;  /* directory always takes 1 addr */
	pinode->addr[0] = block_no;
	pinode->type = 'd'; 
	pinode->mode = '0';				/* ? */
	block_no = pinode->dino;		/* !!!!!!!!!*/
	iput(pinode);					/* release inode */

	/* 
	 * create a new file binding to the block
	 */
	struct file_t new_file;
	strcpy(new_file.name, pathname);
	new_file.dino = block_no;

	/*
	 * add the new directory to the cur dir
	 */
	struct directory_t cur_dir;
	struct inode_t *pinode_cur = iget(cur_dir_inode_no);

	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&cur_dir, 1, sizeof(cur_dir, fd));
	cur_dir.files[cur_dir.size] = new_file;		/* append to it */
	cur_dir.size++;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&cur_dir, 1, sizeof(cur_dir), fd);	/* write back */
	iput(pinode_cur);			/* release inode */

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

	/* search if it is already there */
	unsigned int dino_no = namei(pathname);
	if (dino_no != 0) {
		printf("File or directory exists");
		return;
	}
	
	/* alloc a new inode */
	struct inode_t *pinode = ialloc();
	if (pinode == NULL)
		return;
	pinode->size = 0;
	pinode->type = 'f';
	pinode->mode = '1';

	/* create a file using the block given by inode */
	struct file_t new_file;
	strcpy(new_file.name, pathname);
	new_file.dino = pinode->dino;
	iput(pinode); /* release inode */

	/* add it to the current directory */
	struct inode_t *pinode_cur = iget(cur_dir_inode_no);
	struct directory_t cur_dir;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&cur_dir, 1, sizeof(cur_dit), fd); 
	cur_dir.files[cur_dir.size] = new_file;
	cur_dir.size++;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&cur_dir, 1, sizeof(cur_dir), fd);	/* write back */
	iput(pinode_cur);	/* release inode */

	return;
}

/*
 * remove a file
 *
 *
 */
void rm(const char* pathname) {

	return;
}

void rmdir(const char* pathname) {
	return;
}
