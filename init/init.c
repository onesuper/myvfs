#include <include/fs.h>
#include <stdio.h>
#include <string.h>

/* 
 * simulate the virtual device by creating a binary file 
 * on the disk. All the device operations are replaced by
 * fread/fwrite system calls. 
 *
 */

void format_fs(char* path, char* pwd) {


	FILE *fd = fopen(path, "r+w+b");

	/* 
	 * create a 12 bitmap from #400 to #1000
	 * each bitmap control 50 blocks (#400~#449 ... #1000~1049)
	 */
	struct bmap_t bmap;
	int i, j;
	for (i=NIBLOCK; i<=NIBLOCK + NDBLOCK; i += 50) {
		for(j=1; j<50; j++) {
			bmap.use[j] = 0;
			bmap.addr[j] = i + j + 1;
			bmap.free_block_num = 49;
		}
		fseek(fd, i * SBLOCK, 0);
		fwrite(&bmap, 1, sizeof(bmap), fd);
	}

	printf("bitmap\n");
		
	/* 
	 * write the on-disk inode block on the disk
	 * one by one 
	 */
	struct d_inode_t dino;
	dino.mode = 0;
	dino.no = 1;
	dino.size = 0;
	dino.type = 'e';  /* empty */
	for (i=0; i<NIBLOCK * SBLOCK / sizeof(dino); i++) {
		fseek(fd, SBLOCK * 1 + i * sizeof(dino), 0);					/* the inode block starts at #2 */
		fwrite(&dino, 1, sizeof(dino), fd);		
		dino.no++;							
	}

	printf("on-disk inode\n");

	/* create the user(root) inode */
	dino.size = 1;
	dino.type = 'd';
	dino.dnum = 1;  /* ??? */
	dino.addr[0] = NIBLOCK + 2 + 1;
	dino.no = 1;    /* root inode is $1 inode */
	
	/* write root inode */
	fseek(fd, map_addr(dino.no), 0);
	fwrite(&dino, 1, sizeof(dino), fd);

	/* write root dir */
	struct directory_t dir;
	dir.size = 0;  /* empty at first */
	fseek(fd, SBLOCK * (1 + 2 + NIBLOCK), 0);
	fwrite(&dir, 1, sizeof(dir), fd);

	printf("root dir\n");
	
	/* create usr information at #0 */
	struct user_t usr;
	strcpy(usr.name, "root");  
	strcpy(usr.password, pwd);
	usr.dino = 1;
	fseek(fd, 0 * SBLOCK, 0);
	fwrite(&usr, 1, sizeof(usr), fd);

	printf("user information\n");

	/* create super block at #1 */
	struct super_block_t sb;
	sb.data_block_num = NDBLOCK / 50 * 49;
	sb.inode_block_num = NIBLOCK / 50 * 49;
	sb.free_block_num = NDBLOCK / 50 * 49;
	sb.free_block_sp = 0; /* the stack is empty */
	sb.free_inode_num = NIBLOCK * SBLOCK / sizeof(dino);
	sb.free_inode_sp = 0;
	fseek(fd, SBLOCK * 1, 0);  
	fwrite(&sb, 1, sizeof(sb), fd);

	printf("superblock\n");



	/* close the file handler*/
	fclose(fd);

	return;
}

/* find the position of d_inode according to the no */
unsigned long map_addr(unsigned int no) {
	struct d_inode_t dino;
	unsigned long addr = 2 + sizeof(dino) * (no - 1);
	return addr;
}
