/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 */


#include <include/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * convert a pathname to the d_inode no
 *
 * three pathname is acceptable in this version:
 * 1. root pathname: /
 * 2. current dir pathname: .
 * 3. a dir or file's pathname: {name}
 *
 * if none can be found, return 0 
 *
 */
unsigned int namei(const char *pathname) {
	if (strcmp(pathname, "/") == 0) { /* root path */
		return usr.dino;
	} else if (strcmp(pathname, ".") == 0) { /* current directory */
		return cur_dir_inode_no;
	} else {
		/*
		 * read the dir of current directory
		 * the directory file is at the block
		 * given by pinode->addr[0]
		 *
		 * get the cur_dir_inode in memory first 
		 */
		struct inode_t* pinode = iget(cur_dir_dinode_no);
		struct directory_t dir;
		fseek(fd, pinode->addr[0] * SBLOCK, 0);
		fread(&dir, 1, sizeof(dir), fd);   
		iput(pinode);

		/*
		 * searching the dirs under cur_dir until
		 * find the matched one
		 */
		int i;
		for (i=0; i<dir.size; i++) {
			if (strcmp(dir.files[i].name, pathname) == 0) {
				return dir.files[i].dino;
			}
		}

		/* not found */
		return 0;
	}

}
