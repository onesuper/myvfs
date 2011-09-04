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

	/* if it is ready there */
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
	struct inode_t* pinode = ialloc();
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
	struct directory_t dir_cur;
	struct inode_t *pinode_cur = iget(cur_dir_inode_no);

	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&dir_cur, 1, sizeof(dir_cur), fd);
	dir_cur.files[dir_cur.size] = new_file;		/* append to it */
	dir_cur.size++;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&dir_cur, 1, sizeof(dir_cur), fd);	/* write back */
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

	/* if it is already there */
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
	struct directory_t dir_cur;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&dir_cur, 1, sizeof(dir_cur), fd); 
	dir_cur.files[dir_cur.size] = new_file;
	dir_cur.size++;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&dir_cur, 1, sizeof(dir_cur), fd);	/* write back */
	iput(pinode_cur);	/* release inode */

	return;
}

/*
 * remove a file from the vfs:
 *
 * 1.process the currect diretory
 * 2.release the block it takes
 * 3.free the on-disk inode
 */
void rm(const char* pathname) {

	/* get the dinode no via namei() */
	unsigned int dinode_no = namei(pathname);

	/* can not find */
	if (dinode_no == 0) {
		printf("No such file or directory");
		return;
	}

	struct inode_t* pinode = iget(dinode_no);
	
	/* not a file but a directory */
	if (pinode->type != 'f') {
		printf("Is a directory");
		iput(pinode);		/* release inode */
		return;
	}
	iput(pinode);  /* release the inode */
	
	/* 
	 * check the o_file[100]
	 *
	 * if the file is being opened, denied to remove
	 */
	int i; 
	for (i=0; i<100; i++) {
		if (open_file[i].count > 0 && open_file[i].pinode->dino == dinode_no) {
			printf("the file is already opened");
			return;
		}
	}

	/* get the current dir */
	struct inode_t* pinode_cur = iget(cur_dir_inode_no);
	struct directory_t dir_cur;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&dir_cur, 1, sizeof(dir_cur), fd);

	/* 
	 * delete the exact dinode under the current directory
	 * by searching all the nodes below it
	 */
	for (i=0; i<dir_cur.size; i++) {
		if (dir_cur.files[i].dino == dinode_no) {
			dir_cur.size--;
			dir_cur.files[i] = dir_cur.files[dir_cur.size];
			/* move the stack-top element to the dead one's
			 * place
			 */
			break; /* stop searching */
		}
	}
	
	/* write back the current directory */
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&dir_cur, 1, sizeof(dir_cur), fd);
	iput(pinode_cur);	/* release the inode */

	/* release the blocks the inode takes */
	pinode = iget(dinode_no);
	for (i=0; i<pinode->size; i++) {
		bfree(pinode->addr[i]);
	}
	iput(pinode);
	
	/* free the on disk inode  */
	ifree(dinode_no);

	return;
}

/*
 * remove a directory from the vfs
 * 1.disappear from the current dir
 * 2.
 *
 */
void rmdir(const char* pathname) {

	/* refuse to remove the root dir */
	if (strcmp("/", pathname))
		return;
	
	/* can not find */
	unsigned int dinode_no = namei(pathname);
	if (dinode_no == 0) {
		printf("No such file or directory");
		return;
	}
	
	/* not a directory but a file */
	struct inode_t* pinode = iget(dinode_no);
	if (pinode->type != 'd') {
		printf("Is a file");
		iput(pinode);		/* release inode */
		return;
	}
	iput(pinode);  /* release the pinode */

	/* get current dir */
	struct inode_t* pinode_cur = iget(cur_dir_inode_no);
	struct directory_t dir_cur;
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fread(&dir_cur, 1, sizeof(dir_cur), fd);
	
	/* 
	 * delete the exact dinode under the current directory
	 * by searching all the nodes below it
	 */
	int i;
	for (i=0; i<dir_cur.size; i++) {
		if (dir_cur.files[i].dino == dinode_no) {
			dir_cur.size--;
			dir_cur.files[i] = dir_cur.files[dir_cur.size];
			/* move the stack-top element to the dead one's
			 * place
			 */
			break; /* stop searching */
		}
	}

	/* write back the current dir */
	fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
	fwrite(&dir_cur, 1, sizeof(dir_cur), fd);
	iput(pinode_cur);	/* release the inode */
	
	/* free the block containing the directory */
	pinode = iget(dinode_no);
	struct directory_t dir;
	fseek(fd, pinode->addr[0] * SBLOCK, 0);
	fread(&dir, 1, sizeof(dir), fd);
	bfree(pinode->addr[0]); /* free the #block_no */
	iput(pinode);

	/* free the on-disk inode */
	ifree(dinode_no);

	return;
}
