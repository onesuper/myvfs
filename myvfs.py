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
import subprocess

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
create a subprocess of vi to edit the file
'''
def vi(path = ""):
	if path == "":
		print "The commmand is 'vi xxx'"				
	else:
		# read the file from myvfs
		my_fd = fs.mopen(path)
		if my_fd < 0:
			return

		# change the return type of
		fs.mread.restype = c_char_p
		read_content = fs.mread(my_fd)
		fs.mclose(my_fd)
		
		# use sytem's vi to edit
		#=================================
		# write the file to tempfile
		fd = open("tempfile", "w")
		fd.write(read_content)
		fd.close()

		# use vi to edit the tempfile
		subprocess.call(["vi", "tempfile"])

		# read the file from the tempfile
		fd = open("tempfile", "r")
		write_content = fd.read()
		fd.close()
		#=================================

		# write the file to myvfs
		my_fd = fs.mopen(path)
		fs.mwrite(my_fd, c_char_p(write_content))
		fs.mclose(my_fd)

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



	def do_vi(self, path = ""):
		if path == "":
			vi()
		else:
			if check_pathname(path):
				vi(path)
			else:
				path_error()
	



	"""document for help
	"""

	def help_exit(self):
		print "quit myvfs quietly"

	def help_mkdir(self):
		print "make a new directory"

	def help_ls(self):
		print "list the directories and files in the current directory"
	
	def help_rmdir(self):
		print "remove a directory"

	def help_rm(self):
		print "remove a file"

	def help_touch(self):
		print "create a file"

	def help_cd(self):
		print "change to a directory"
	
	def help_vi(self):
		print "a text editor to view and edit the file"

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
