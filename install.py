'''
Version: 0.1
Author: onesuper
E-mail: onesuperclark@gmail.com

This script will install a new virtual file system on your disk
'''
#!/usr/bin/python
# Filename: install.py


import sys
import os
import getpass
from ctypes import *

'''local function for common use
'''

def Terminate():
	print '\nThe installation has been terminated.'
	sys.exit()




if __name__ == "__main__":


	
	# setting up the root password
	print "Before installing the virtual file system, you're required to set up a password for login(as root)."
	password = ""
	try_times = 1
	while True:
		if try_times > 3:
			Terminate()
		pwd1 = getpass.getpass("Please enter a password:\n")
		pwd2 = getpass.getpass("Again:\n")
		if pwd2 == pwd1 and pwd1 != "":
			password = pwd1
			break
		else:
			print ("Not match or empty(%d)") % try_times
			try_times += 1

	

	print '''You are going to install a virtual file system on your disk.'''

	while True:	
		YesOrNo = raw_input("Contine?(y/N):")
		if YesOrNo == "y" or YesOrNo == "Y":
			break
		elif YesOrNo == "n" or YesOrNo == "N":
			Terminate()

	''' Now start creating a vfs on the installation path.
A shared library named 'init.so' is needed here! '''

	init = cdll.LoadLibrary(os.getcwd() + '/init/init.so')

	# initialize the vfs and intall to disk
	init.format_fs("volume1.vfs", password) 

	# all things done
	print "installed successfully!"
