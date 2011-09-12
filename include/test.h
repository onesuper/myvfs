/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * head file for unit test
 */

extern void format_fs(char*, char*);
extern void mount(char*);
extern void umount(void);
extern void ls(void);
extern void cd(const char*);
extern void mkdir(const char*);
extern void touch(const char*);
extern void rm(const char*);
extern void rmdir(const char*);
extern int	mopen(const char*);
extern void mclose(int);
extern char* mread(int);
extern void	mwrite(int, const char*);

/* util */
extern void picture(void);
extern void watch_dinode(unsigned int);
extern void watch_inode(struct inode_t*);
extern void watch_usr(void);
