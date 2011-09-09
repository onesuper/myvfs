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
	int fd = open("ppg");
	write(fd, "good morning!");
	char* out = read(fd);
	printf(out);
	close(fd);
	umount();
	return 0;
}
