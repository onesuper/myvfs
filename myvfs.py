'''
Version: 0.1
Author: onesuper
E-mail: onesuperclark@gmail.com

a friendly shell powered by Python's cmd module 

the main system calls are encapsulated as a dynamic
link library (fs.so)

use Python's ctypes module to call a C-function
in this program

'''
#!/usr/bin/python
# Filename: myvfs.py


import sys
import os
import re
from ctypes import *
import getpass
import cmd
'''local function for common use
'''

def quit_quietly():
	'''unmount the vfs
	'''
	fs.umount()
	sys.exit()

def check_pathname(pathname):
	if re.match(r"^[a-zA-Z0-9._]{1,16}$", pathname):
		return True
	else:
		return False

def path_error():
	print 'Invalid directory or file name'

'''
Command-line Class
'''
class Shell(cmd.Cmd):
	intro = '''myvfs 0.1.0
Type "help" or "?" for more information.'''
	
	doc_header = "Type <command> to see the instruction."
	undoc_header = "other commands"

	prompt = "myvfs> "
	ruler = "="

	def emptyline(self):
		'''if the input is an empty line
		'''
		return
	
	'''
	commands
	'''
	def do_exit(self, path):
		quit_quietly()

	def do_ls(self, path):
		fs.ls()

	def do_mkdir(self, path):
		if check_pathname(path):
			fs.mkdir(path)
		else:
			path_error()
	
	def do_rmdir(self, path):
		if check_pathname(path):
			fs.rmdir(path)
		else:
			path_error()

	def do_cd(self, path):
		if check_pathname(path):
			fs.cd(path)
		else:
			path_error()

	def do_touch(self, path):
		if check_pathname(path):
			fs.touch(path)
		else:
			path_error()
	
	def do_rm(self, path):
		if check_pathname(path):
			fs.rm(path)
		else:
			path_error()
	
	def do_open(self, path):
		print path

	def do_close(self, path):
		print path

	def do_read(self, path):
		print path

	def do_write(self, path):
		print path

	"""document for help
	"""

	def help_exit(self):
		print "quit myvfs quietly"

	def help_mkdir(self):
		print "make a new directory"

	def help_ls(self):
		print "list the directories and files in the current directory"
	
	def help_rmdir(self):
		print "..."

	def help_rm(self):
		print "..."

	def help_touch(self):
		print "..."

	def help_cd(self):
		print "..."
	
	def help_open(self):
		print "..."

	def help_close(self):
		print "..."

	def help_read(self):
		print "..."

	def help_write(self):
		print "..."

	def help_help(self):
		print "help yourself"



'''The main function is here
'''

if __name__ == "__main__":
	

	# mount the vfs
	fs = cdll.LoadLibrary(os.getcwd() + '/fs/fs.so')
	try:
		fs.mount("volume1.vfs")
		print "mount successfully!"
	except:
		print "fail to mount the vfs!"
		sys.exit()
	
	
	# check in
	try_times = 1
	while True:
		if try_times > 3:
			sys.exit()
		insert = getpass.getpass("Please enter root password:\n")
		if fs.verify(insert) == 1:
			break
		else:
			print "bad password(%d)" % try_times
			try_times += 1


	# start command line loop
	sh = Shell()
	sh.cmdloop()
