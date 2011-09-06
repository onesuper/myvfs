/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * unit test for ls system call
 */

#include <include/fs.h>
#include <include/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	
	printf("mount the vfs\n");
	mount("volume1.vfs");

	printf("call ls\n");
	ls();
	return 0;
}
