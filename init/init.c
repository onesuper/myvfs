/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * simulate the virtual device by creating a binary file 
 * on the disk. All the device operations are replaced by
 * fread/fwrite system calls. 
 */

#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * format all the on-disk structures including:
 *
 * 1. find the vfs and open
 * 2. twelve bitmaps	#402~#1001
 * 3. on-disk inode		#2~#401
 * 4. root dinode		$1
 * 5. root directory	#403
 * 6. usr info			#0
 * 7. super block		#1
 *
 * which can be easily pictured as:
 * user superblock	on-disk inodes						free blocks and bitmap
 * [0]	[1]					[2]................[401]	[402].......................[1001]
 */
void format_fs(char* path, char* pwd) {

	/* open vfs */
	FILE *fd = fopen(path, "r+w+b");
	if (fd == NULL) 
	{
		printf("vfs does not exist\n");
		exit(0);
	}
	printf("find the vfs\n");

	/* 
	 * create 12 bitmaps and store them at
	 * #402		#452	#502	#552	#602	#652
	 * #702		#752	#802	#852	#902	#952
	 *
	 * each bitmap manages the next nearby 49 blocks
	 * e.g. the bitmap stored in #402 manages #403~#451
	 */
	struct bmap_t bmap;
	int i, j;
	for (i=NIBLOCK + 2; i<NIBLOCK + NDBLOCK + 2; i+=50) {  /* [402, 1002) */
		for(j=1; j<50; j++) {		/* [1, 49] */
			bmap.use[j] = 0; 
			bmap.addr[j] = i + j;	/* 1 + 402 = 403, 49 + 402 = 451 */
			bmap.free_block_num = 49;
		}
		fseek(fd, i * SBLOCK, 0);
		fwrite(&bmap, 1, sizeof(bmap), fd);
	}
	printf("bitmap done\n");

	/* 
	 * initialize the on-disk inode(dinode)
	 * dinode's number = size of inode block area / size of dinode
	 * the inode block area starts at #2
	 *
	 * NOTE: THE DINODE.NO STARTS FROM 1 BUT NOT 0
	 */
	struct d_inode_t dinode;
	dinode.dino = 1;
	dinode.size = 0;
	dinode.type = 'e';											/* write the empty inodes from $1 */
	for (i=0; i<SBLOCK * NIBLOCK / sizeof(dinode); i++) {		
		fseek(fd, 2 * SBLOCK + i * sizeof(dinode), 0);		/* the dinode starts at #2 */
		fwrite(&dinode, 1, sizeof(dinode), fd);		
		dinode.dino++;							
	}
	printf("on-disk inode done\n");

	/*
	 * create the user(root)'s directory inode
	 * write it at $1
	 */
	dinode.size = 1;	
	dinode.type = 'd';						/* directory */
	dinode.addr[0] = NIBLOCK + 2 + 1;		/* #403 */
	dinode.dino = 1;						/* root inode is $1 */
	fseek(fd, SBLOCK * map_addr(dinode.dino), 0);
	fwrite(&dinode, 1, sizeof(dinode), fd);

	/* create root dir at #403 */
	struct directory_t dir;
	dir.size = 0;				/* nothing in the root dir at first */
	fseek(fd, SBLOCK * (NIBLOCK + 2 + 1), 0);
	fwrite(&dir, 1, sizeof(dir), fd);
	printf("root dir done\n");

	/* since the #403 is used, adjust the bitmap in #402 */
	fseek(fd, 402 * SBLOCK, 0);
	fread(&bmap, 1, sizeof(bmap), fd);
	bmap.use[1] = 1;
	bmap.free_block_num--;
	fseek(fd, 402 * SBLOCK, 0);
	fwrite(&bmap, 1, sizeof(bmap), fd);

	/* create usr at #0 */
	struct user_t usr;
	strcpy(usr.name, "root");  
	strcpy(usr.password, pwd);
	usr.dino = 1;				/* $1 */
	fseek(fd, 0, 0);
	fwrite(&usr, 1, sizeof(usr), fd);
	printf("user done\n");

	/* create super block at #1 */
	struct super_block_t sb;
	sb.data_block_num = NDBLOCK - ( NDBLOCK / 50 );		/* 600 - 12 = 588 */
	sb.inode_block_num = NIBLOCK;						/* 400 */
	sb.free_block_num = NDBLOCK - ( NDBLOCK / 50 ) - 1;	/* one for root block*/
	sb.free_block_sp = 0;	
	sb.free_inode_num = NIBLOCK * SBLOCK / sizeof(dinode) - 1; /* one for root inode */
	sb.free_inode_sp = 0;
	sb.modified = 0;
	fseek(fd, SBLOCK * 1, 0);  
	fwrite(&sb, 1, sizeof(sb), fd);
	printf("super block done\n");

	/* close the file handler and return */
	fclose(fd);
	return;
}

/*
 * find dinode's physical address according to no
 * since the dinode are alloced sequently
 * if the given no is 1, the addr is 2
 * if the given no is x, the addr is 2 + (x - 1) * sizeof(dinode)
 */
unsigned long map_addr(unsigned int dino) {
	struct d_inode_t dinode;
	unsigned long addr = 2 + sizeof(dinode) * (dino - 1);
	return addr;
}
