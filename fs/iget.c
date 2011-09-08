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
 * get an inode through dinode_no
 */
struct inode_t* iget(unsigned int dinode_no) {

	struct inode_t* pinode;
	int key = dinode_no % NINODE;					/* calculate the key */
	/*
	 * check the inode table first
	 * if the inode isin the hash table
	 * just find and return it
	 */
	if (inode_hash_table[key].forward != NULL) {	
		pinode = inode_hash_table[key].forward;		/* enter the chain */
		while (pinode != NULL) {
			if (pinode->dino == dinode_no) {		
				pinode->count++;					/* increase the reference count */
				return pinode;					
			} else {
				pinode = pinode->forward;			/* next */
			}
		}
	}

	/*
	 * if the inode is not in the hash table
	 * create a new one and add to it
	 */
	pinode = (struct inode_t*)malloc(sizeof(struct inode_t));	/* create by malloc() */
	d_to_inode(pinode, dinode_no);

	/* 
	 * add to hash table
	 * 
	 * [key]<-->[new]<-->[inode]
	 * [key]<-->[new]
	 */
	pinode->forward = inode_hash_table[key].forward;
	pinode->backward = &inode_hash_table[key];		
	if (pinode->forward != NULL) {
		pinode->forward->backward = pinode;
	}
	inode_hash_table[key].forward = pinode;

	/* set inode's state */
	pinode->count = 1;		/* a fairly new one */
	pinode->flag = 0;		/* update?? */

	return pinode;
}


/* 
 * reduce the reference count
 */
void iput(struct inode_t *pinode) {


	/*
	 * if the inode is still referenced by other file
	 * do not have to release it
	 */
	if (pinode->count > 1) {
		pinode->count--;
		return;
	}
	
	/*
	 * store the inode to the disk first
	 */
	i_to_dinode(pinode);

	/*
	 * else the inode must be removed from the hash table
	 */
	if (pinode->forward == NULL) {		/* inode in the end of chain */
		pinode->backward->forward = NULL;
		free(pinode);
		return;
	} else {			/* in the middle */
		pinode->forward->backward = pinode->backward;
		pinode->backward->forward = pinode->forward;
		free(pinode);
		return;
	}
}
