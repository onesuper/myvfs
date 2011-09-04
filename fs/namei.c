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
 * convert a pathname to an on-disk inode no
 *
 * if none inode can be found, return 0 
 *
 */
unsigned int namei(const char *pathname) {
	if (strcmp(pathname, "/") == 0) { /* root path */
		return usr.dino;
	} else if (strcmp(pathname, ".") == 0) { /* current directory */
		return cur_dir_inode_no;
	} else {
		/* get the current dir inode via cur_dir_inode_no */
		struct inode_t* pinode_cur = iget(cur_dir_inode_no);

		/* read the dir out */
		struct directory_t dir;
		fseek(fd, pinode_cur->addr[0] * SBLOCK, 0);
		fread(&dir, 1, sizeof(dir), fd);   
		
		/* */
		iput(pinode_cur);

		/* */
		int i;
		for (i=0; i<dir.size; i++) {
			if (strcmp(dir.files[i].name, pathname) == 0) {
				return dir.files[i].dino;
			}
		}

		/* no found */
		return 0;
	}

}
