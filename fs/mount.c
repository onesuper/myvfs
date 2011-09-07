/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 */

#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * global variables are defined below,
 * which will be used in other place in the program
 * until the vfs is umounted
 */
struct super_block_t	sb;
struct inode_p_t		inode_hash_table[NINODE];	/* in-core inode table */
struct open_file_t		open_file[NOFILE];			/* open-file table */
unsigned short			o_file_num;					/* num of open file */
struct user_t			usr;						/* one user in system-root*/
unsigned int cur_dir_dinode_no;			/* the dinode no of current directory */ 
FILE *fd;								/* point to the vfs file*/

/* 
 * load the information of vfs into memory including:
 * 1. superblock
 * 2. user
 * 3. current dir
 *
 * and get some in-core stuff ready including:
 * 1. in-core inode table (hashed)
 * 2. open file table
 */
void mount(char* path) {

	/* open the vfs on the disk */
	fd = fopen(path, "r+w+b");
	if (fd == NULL) {
		printf("vfs file cannot be opened\n");
		exit(0);
	}

	/* read the user information */
	fseek(fd, 0, 0);
	fread(&usr, 1, sizeof(usr), fd);
	printf("usr done\n");

	/* set the current dir, $1 */
	cur_dir_dinode_no = usr.dino;

	/* read the superblock*/
	fseek(fd, 1 * SBLOCK, 0);
	fread(&sb, 1, sizeof(sb), fd);
	printf("superblock done\n");
	
	/* initialize the in-core inode table */
	int i;
	for (i=0; i<NINODE; i++) {
		inode_hash_table[i].forward = NULL;
	}
	printf("inode table done\n");

	/* initialize the open-file table */
	for (i=0; i<NOFILE; i++) {
		open_file[i].count = 0;
		open_file[i].pinode = NULL;
	}
	printf("open file done\n");

	return;
}


