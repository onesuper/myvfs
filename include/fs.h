#include "param.h"
#include <stdio.h>


/***************************************************************
 *    on disk data structures
 ***************************************************************/


/* bmap */
struct bmap_t {
	char			use[49];		/* 1 for used, 0 for free */
	unsigned int	addr[49];		/* physical address of each block */
	unsigned short	free_block_num;         /* number of free blocks */          
};

/* on disk inode */
struct d_inode_t {
	char			type;		/* e for empty, d for data*/
	unsigned short	dnum;
	unsigned int	no;
	unsigned short	mode;		/* 0 for data, 1 for directory */
	unsigned int	size;
	unsigned int	addr[50];	/* physical block no */
};

/* super block */
struct super_block_t {
	
	unsigned int	inode_block_num;			/* number of inode blocks */
	unsigned int	data_block_num;				/* number of data blocks */
	unsigned int	free_block_num;				/* number of free blocks */
	unsigned int	free_block_stack[NINODE];	/* stack of free block */
	unsigned short	free_block_sp;				/* stack pointer */
	
	unsigned int	free_inode_num;				/* number of free inode */
	unsigned int	free_inode_stack[NINODE];	/* stack of free inode */
	unsigned short	free_inode_sp;				/* stack pointer */

};

/* file on the disk */
struct file_t {
	unsigned int	dino;	  /* on disk inode number */
	char			name[24]; /* in unix is 14 */ 
};


/* directory entries */
struct directory_t {
	unsigned short	size;
	struct file_t	files[36]; /* in unix is 32 which is related
								  to the 512 bytes */
};

/* user */
struct user_t {
	char			name[12];
	char			password[12];
	unsigned int	dino;			/* the root d_inode of user */
};


/**********************************************************
 *     in core data structures
 **********************************************************/


/* in core inode */
struct inode_t {
	struct inode_t* forward;    /* forward inode */
	struct inode_t* backward;	/* backward inode */
	unsigned int	no;		
	unsigned int	count;		/* referece count */
	unsigned short  flag;		/* updated ?*/
	char			type;
	unsigned int	dino;		/* disk inode no */	
	unsigned short	dnum;		/* dino physical block num */

	unsigned short	mode;		/* */
	unsigned int	size;		
	unsigned int	addr[50];	/* physical block no */
};

/* 
 * struct to construct the hash table
 * which leading a chain like:
 *
 * struct inode_p_t inode_hash_table[128]
 *
 * [inode_p_0] -> (inode) -> (inode)
 * [inode_p_1] -> (inode)
 * [inode_p_2] -> (inode) 
 * ...
 */
struct inode_p_t {
	struct inode_t *inode;
};

/* opening file */
struct o_file_t {
	char			flag;
	unsigned int	count;
	int				inode;      /* point to the in core inode, -1 for NULL */
	int				uid;
	unsigned int	offset;
};


/************************************************
 *	  extern   declaration
 ************************************************/

/* mount.c */
extern FILE* fd;
extern struct super_block_t sb;
extern struct inode_p_t inode_table[NINODE];
extern struct o_file_t o_file[NFILE];
extern struct user_t usr;				

/* util.c */
extern void save_super_block(void);
extern unsigned long map_addr(unsigned int);

/* alloc.c */
extern struct inode_t* ialloc(void);
extern void ifree(unsigned int);

/* iget.c */
extern struct inode_t* iget(unsigned int);
extern void iput(struct inode_t*);

/* namei.c */
extern unsigned int namei(const char*);

