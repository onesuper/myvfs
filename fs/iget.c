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
 * get the in-core inode via the on-disk inode's id.
 *	
 * the function  is called when a file is opened.
 */
struct inode_t* iget(unsigned int dino_id) {

	/* 
	 * if the inode is already in the hash table
	 * try to find and return it
	 * get the key first and look up by the chain
	 */
	struct inode_t *temp_inode;
	int key = dino_id % NINODE;

	if (inode_hash_table[key].forward != NULL) {
		temp_inode = inode_hash_table[key].forward;
		while (temp_inode) {
			if (temp_inode->no == key) { /* ?? */
				/* find! */
				temp->count++; /* increase the reference count */
				return temp_inode;
			} else {
				temp_inode = temp->forward; 
			}
		}
	}


	/*
	 * if the inode is not in the hash table:
	 *
	 * 1.read it from disk and create a in-core inode
	 * and return it.
	 *
	 * 2.add it into hash table.
	 *
	 * 3.update the flags.
	 */
	struct inode_t *new_inode;
	struct d_inode_t dino;
	/* 1 */
	addr = map_addr(dino_id);
	new_inode = (struct inode_t)malloc(sizeof(new_inode), fd);
	fseek(fd, addr, 0);
	fread(&new_inode->dnum, 1, sizeof(dino), fd); /* ?? */
	/*
	 * 2
	 *
	 * before:
	 * [key1]->(inode1)->(inode2)
	 *
	 * after:
	 * [key1]->(new_inode)->(inode1)->(inode2)
	 */
	new_inode->forward = inode_hash_table[key].forward;
	new_inode->backward = new_inode;
	if (new_inode->foward != NULL) {
		new_inode->foward->backward = new_inode;
	}
	inode_hash_table[key].forward = new_inode;
	/* 3 */
	new_inode->count = 1;  /* a fairly new one */
	new_inode->flag = 0;
	new_inode->no = key; /* ?????? */



	return new_inode;
}


/* 
 * pinode point to the existing inode in the hash table
 * which belongs to the openning file.
 *
 * the function is called when a file is closed.
 *
 * it will delete a in-core inode and reduce the reference
 * count.
 *
 */
void iput(struct inode_t *pinode) {

	/* find the */
	unsigned long addr;
	struct d_inode_t dino;
	addr = map_addr(pinode->dino);
	fseek(fd, addr, 0);
	fwrite(&pinode->dnum, 1, sizeof(dino), fd); /* */
	
	/*
	 * if the inode is still referenced by other file
	 * do not have to release it
	 */
	if (pinode->count > 1) {
		pinode->count--;
		return;
	}
	/*
	 * else the inode must be removed from the hash table
	 */
	if (pinode->forward == NULL) {
		/*
		 * before:
		 * [key1]->(inode1)->(inode2)->(target_inode)
		 *
		 * after:
		 * [key1]->(inode1)->(inode2)->NULL
		 */
		pinode->backward->forward = NULL;
		free(pinode);
		return;
	} else {
		/*
		 * before:
		 * [key1]->(inode1)->(taget_inode)->(inode2)
		 *
		 * after:
		 * [key1]->(inode1)->(inode2)
		 *
		 */
		pinode->forward->backward = pinode->backward;
		pinode->backward->forward = pinode->forward;
		free(pinode);
		return;
	}

}
