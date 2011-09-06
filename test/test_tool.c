/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * tools for testing 
 */

#include <include/fs.h>
#include <include/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * picture the vfs
 */
void picture(void) {

	printf("#######################################\n");
	/* super block */
	struct super_block_t sb;
	fseek(fd, SBLOCK * 1, 0);  
	fread(&sb, 1, sizeof(sb), fd);
	printf("superblock\n");
	printf("data_block_num: %d\n", sb.data_block_num);
	printf("inode_block_num: %d\n", sb.inode_block_num);
	printf("free_block_num: %d\n", sb.free_block_num);
	printf("free_block_sp: %d\n", sb.free_block_sp);
	printf("free_inode_num: %d\n", sb.free_inode_num);
	printf("free_inode_sp: %d\n", sb.free_inode_sp);
	printf("modified: %d\n", sb.modified);
	printf("=============================================\n");

	/* usr */
	printf("usr\n");
	struct user_t usr;
	fseek(fd, 0, 0);
	fread(&usr, 1, sizeof(usr), fd);
	printf("name: %s\n", usr.name);
	printf("password: %s\n", usr.password);
	printf("dino: %d\n", usr.dino);
	printf("============================================\n");
	
	/* dinode */
	printf("dinode\n");
	struct d_inode_t dinode;
	int i;			
	for (i=0; i<SBLOCK * NIBLOCK / sizeof(dinode); i++) {		
		fseek(fd, 2 * SBLOCK + i * sizeof(dinode), 0);				
		fread(&dinode, 1, sizeof(dinode), fd);	
		printf("dino: %d\n", dinode.dino);
		printf("type: %c\n", dinode.type);
		printf("addr[0]: %d\n", dinode.addr[0]);
		printf("size: %d\n", dinode.size);
		printf("----------------------\n");
		dinode.dino++;							
	}
	printf("============================================\n");

	/* bitmap */
	struct bmap_t bmap;
	printf("bitmap\n");
	int j;
	for (i=NIBLOCK + 2; i<NIBLOCK + NDBLOCK + 2; i+=50) {  
		fseek(fd, i * SBLOCK, 0);
		fread(&bmap, 1, sizeof(bmap), fd);
		for(j=1; j<50; j++) {		
			printf("\t%d ", bmap.use[j]); 
			printf("%d ", bmap.addr[j]);	
			printf("%d ", bmap.free_block_num = 49);
		}
		printf("\n-------------------------------------------------------------------\n");
	}
	printf("=============================================\n");


	return;
}
