'''
Version: 0.1
Author: onesuper
E-mail: onesuperclark@gmail.com

a friendly shell powered by Python's cmd module 

the main system calls are encapsulated as a dynamic
link library (fs.so)

use Python's ctypes module to call a c function
in Python program.

'''
#!/usr/bin/python
# Filename: shell.py


import sys
import os
import re
from ctypes import *
import getpass
import cmd
'''local function for common use
'''

def quit_quietly():
	# unmount the vfs
	sys.exit()



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
		return
	

	'''
	commands
	'''
	def do_exit(self, path):
		print 'exit'

	def do_ls(self, path):
		print 'ls'

	def do_mkdir(self, path):
		print path
	
	def do_rmdir(self, path):
		print path

	def do_cd(self, path):
		print path

	def do_touch(self, path):
		print path
	
	def do_rmfile(self, path):
		print path
	
	def do_open(self, path):
		print path

	def do_close(self, path):
		print path

	def do_look(self, path):
		print path

	"""document for help
	"""

	def help_exit(self):
		print "quit myvfs quietly"

	def help_mkdir(self):
		print "make a new directory"

	def help_ls(self):
		print "..."
	
	def help_look(self):
		print "..."

	def help_close(self):
		print "..."

	def help_rmdir(self):
		print "..."

	def help_rmfile(self):
		print "..."

	def help_touch(self):
		print "..."

	def help_cd(self):
		print "..."
	
	def help_open(self):
		print "..."

	def help_help(self):
		print "..."

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

	

