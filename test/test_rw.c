/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * unit test for touch
 */

#include <include/fs.h>
#include <include/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	mount("volume2.vfs");
	ls();
	touch("ppg");
	ls();
	int fd;
	fd = mopen("ppg");
	char *out = mread(fd);
	printf(out);
	mclose(fd);

	fd = mopen("ppg");
	mwrite(fd, "spider man!");
	mclose(fd);
	
	fd = mopen("ppg");
	out = mread(fd);
	printf(out);
	mclose(fd);
	umount();
	return 0;
}
