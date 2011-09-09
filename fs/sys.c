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
 * list the all the files and directories
 * under current directory
 */
void ls(void) {

	/*
	 * in order to see what is in the directory entries
	 * get it by iget(), and then drop it by iput() 
	 */
	struct directory_t dir;
	struct inode_t* pinode= iget(cur_dir_dinode_no);
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd);
	iput(pinode);
	
	/* 
	 * delete the target dinode under the current directory
	 * by searching all the nodes below it
	 */

	/* print */
	int i;
	for (i=0; i<dir.size; i++) {
		printf("%s\t", dir.files[i].name);
		if (((i + 1) % 5) == 0)
			printf("\n");
	}
	/* look better */
	if ((dir.size % 5) != 0)
		printf("\n");

	return;
}


/*
 * change to a directory
 *
 */
void cd(const char* pathname) {
	
	/*
	 * get pinode in memory
	 */
	int dinode_no = namei(pathname);
	if (dinode_no == 0) {
		printf("No such file or directory\n");
		return;
	}
	struct inode_t* pinode = iget(dinode_no);
	if (pinode == NULL) {
		printf("No such file or directory\n");
		return;
	}
	if (pinode->type != 'd') {
		printf("Not a directory\n");
		return;
	}
	
	/* change the cur dir */
	cur_dir_dinode_no = dinode_no;

	return;
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
	
	/* ensure not exist */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no != 0) {
		printf("File or directory exists\n");
		return;
	}
		
	/* 
	 * create a new directory structure
	 * an empty dir has one entry which 
	 * represents its upper layer dir
	 * in this case cur dir
	 */
	struct directory_t dir;
	dir.size = 1;
	strcpy(dir.files[0].name, "..");
	dir.files[0].dino = cur_dir_dinode_no;

	/* 
	 * allocate a block and an inode
	 * which is a atomic operation
	 */
	unsigned int block_no = balloc();  
	if (block_no == 0)
		return;	
	struct inode_t* pinode = ialloc();
	if (pinode == NULL) {
		bfree(block_no);  /* rollback~ */
		return;
	}

	/* 
	 * write the directory to the block
	 * and register inode info
	 */
	unsigned int new_dinode_no;
	fseek(fd, block_no * SBLOCK, 0);
	fwrite(&dir, 1, sizeof(dir), fd);
	pinode->size = 1;				/* new dir's size */
	pinode->addr[0] = block_no;
	pinode->type = 'd'; 
	new_dinode_no = pinode->dino;	/* remember the dino */
	iput(pinode);					/* release inode after using it */

	/* 
	 * create a file structure to record the 
	 * dinode_no and the pathname
	 */
	struct file_t file;
	strcpy(file.name, pathname);
	file.dino = new_dinode_no;

	/*
	 * add the new directory to the cur dir
	 * which means adding the file to it's files array
	 */
	struct directory_t dir_cur;
	struct inode_t *pinode_cur = iget(cur_dir_dinode_no);
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&dir_cur, 1, sizeof(dir_cur), fd);
	dir_cur.files[dir_cur.size] = file;				/* append the file to it */
	dir_cur.size ++;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&dir_cur, 1, sizeof(dir_cur), fd);		/* write back the dir to block*/
	iput(pinode_cur);								/* release inode */

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

	/* ensure not exist */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no != 0) {
		printf("File or directory exists\n");
		return;
	}
	
	/* 
	 * create a file using allocated dinode 
	 * then allocate a new inode
	 */
	struct inode_t *pinode = ialloc();
	if (pinode == NULL)
		return;
	pinode->size = 0;
	pinode->type = 'f';
	struct file_t file;
	strcpy(file.name, pathname);
	file.dino = pinode->dino;	/* binding the file with dinode */
	iput(pinode);				/* release inode */

	/* add the file to the current directory */
	pinode = iget(cur_dir_dinode_no);
	struct directory_t dir;
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd); 
	dir.files[dir.size] = file;
	dir.size ++;
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fwrite(&dir, 1, sizeof(dir), fd);		/* write back */
	iput(pinode);							/* release inode */

	return;
}

/*
 * remove a file from the vfs:
 */
void rm(const char* pathname) {

	/* ensure present */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no == 0) {
		printf("No such file or directory\n");
		return;
	}
		
	/* not a file but a directory */
	struct inode_t* pinode = iget(dinode_no);
	if (pinode->type != 'f') {
		printf("Is a directory\n");
		iput(pinode);
		return;
	}
	iput(pinode);  /* release the inode */
	
	/* 
	 * check the open_file[100]
	 * if the file is being opened, denied to remove
	 */
	int i; 
	for (i=0; i<100; i++) {
		if (open_file[i].count > 0 && open_file[i].pinode->dino == dinode_no) {
			printf("the file is already opened\n");
			return;
		}
	}

	/* 
	 * get the current dir, then delete the target
	 * dinode under the current directory by searching
	 * all the nodes below it
	 */
	pinode = iget(cur_dir_dinode_no);
	struct directory_t dir;
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd);
	for (i=0; i<dir.size; i++) {
		if (dir.files[i].dino == dinode_no) {
			/* move the stack-top element to the dead one's
			 * place, and the stack shrink
			 */
			dir.size--;
			dir.files[i] = dir.files[dir.size];
			break; /* stop searching */
		}
	}
	/* write back the current dir */
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fwrite(&dir, 1, sizeof(dir), fd);
	iput(pinode);

	/* free the blocks of the target file */
	pinode = iget(dinode_no);
	for (i=0; i<pinode->size; i++) {
		bfree(pinode->addr[i]);
	}
	iput(pinode);
	
	/*  finally free the d_inode  */
	ifree(dinode_no);
	
	return;
}

/*
 * remove a directory from the vfs
 */
void rmdir(const char* pathname) {

	/* refuse to remove the root dir */
	if (strcmp("/", pathname) == 0)
		return;
	
	/* refuse to remove the upper layer dir */
	if (strcmp("..", pathname) == 0)
		return;

	/* ensure present */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no == 0) {
		printf("No such file or directory\n");
		return;
	}
	
	/* not a directory but a file */
	struct inode_t* pinode = iget(dinode_no);
	if (pinode->type != 'd') {
		printf("Is a file\n");
		iput(pinode);		/* release inode */
		return;
	}
	/* 
	 * make sure it is directory and count
	 * the files under it. if it has two or 
	 * more files, refuse to remove it
	 */
	struct directory_t dir;
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd);
	if (dir.size > 1) {
		printf("Directory not empty\n");
		iput(pinode);
		return;
	}
	iput(pinode);  /* release the pinode */

	/* 
	 * get current dir, then delete the target dinode
	 * under the current directory by searching all 
	 * the nodes below it
	 */
	pinode = iget(cur_dir_dinode_no);
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd);
	int i;
	for (i=0; i<dir.size; i++) {
		if (dir.files[i].dino == dinode_no) {
			/* 
			 * move the stack-top element to the dead one's
			 * place
			 */
			dir.size--;
			dir.files[i] = dir.files[dir.size];
			break; /* stop searching */
		}
	}
	/* write back the current dir */
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fwrite(&dir, 1, sizeof(dir), fd);
	iput(pinode);	/* release the inode */
	
	/* free the block of the target dir */
	pinode = iget(dinode_no);
	bfree(pinode->addr[0]);			 /* free the #block_no */
	iput(pinode);

	/* free the target dinode */
	ifree(dinode_no);
	
	return;
}
