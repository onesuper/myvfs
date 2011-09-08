/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * main data structures are defined in this head file
 *
 */


#include "param.h"
#include <stdio.h>

/***************************************************************
 *    on disk data structures
 ***************************************************************/


/* bmap */
struct bmap_t {
	char			use[50];		/* 1 for used, 0 for free */
	unsigned int	addr[50];		/* physical address of each block */
	unsigned short	free_block_num; /* number of free blocks */          
};

/* on disk inode */
struct d_inode_t {
	char			type;			/* 'e'mpty, 'd'ir, 'f'ile, 'u'sed */
	unsigned int	dino;			/* dinode no */
	unsigned int	size;			/* how many blocks */
	unsigned int	addr[50];		/* physical block no */
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
	char			modified;					/* whether ths sb has been
												   modified */
};

/* file on the disk */
struct file_t {
	unsigned int	dino;					/* on disk inode number */
	char			name[24];				/* unix's is 14 */ 
};


/* directory entries */
struct directory_t {
	unsigned short	size;
	struct file_t	files[36];			/* in unix is 32 which is related
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
	struct inode_t* forward;			/* forward inode */
	struct inode_t* backward;			/* backward inode */
	unsigned int	count;				/* referece count */
	unsigned short  flag;				/* updated ?*/
	/* dinode */
	char			type;			
	unsigned int	dino;				/* dinode no */
	unsigned int	size;				/* how many blocks */
	unsigned int	addr[50];			/* physical block no */
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

/* opening file */
struct open_file_t {
	char				flag;
	unsigned int		count;
	struct inode_t*		pinode;      /* point to the in core inode */
	unsigned int		offset;
};


/************************************************
 *	  extern   declaration
 ************************************************/

/* mount.c */
extern FILE* fd;
extern struct super_block_t		sb;
extern struct inode_t			inode_hash_table[NINODE];
extern struct open_file_t		open_file[NOFILE];
extern unsigned short			o_file_num;
extern struct user_t			usr;
extern unsigned int				cur_dir_dinode_no;

/* util.c */
extern void save_super_block(void);
extern unsigned long map_addr(unsigned int);
extern void d_to_inode(struct inode_t* , unsigned int);
extern void i_to_dinode(struct inode_t*, unsigned int);

/* alloc.c */
extern struct inode_t* ialloc(void);
extern void ifree(unsigned int);
extern unsigned int balloc(void);
extern void bfree(unsigned int);

/* iget.c */
extern struct inode_t* iget(unsigned int);
extern void iput(struct inode_t*);

/* namei.c */
extern unsigned int namei(const char*);

