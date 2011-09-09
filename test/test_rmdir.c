/*
 * Version: 0.1
 * Author: onesuper
 * Email: onesuperclark@gmail.com
 *
 * unit test for rmdir
 */

#include <include/fs.h>
#include <include/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	mount("volume2.vfs");
	ls();
	mkdir("fm2011");
	ls();
	rmdir("fm2011");
	ls();
	umount();
	return 0;
}
