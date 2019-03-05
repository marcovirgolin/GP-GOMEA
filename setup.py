#!/usr/bin/env python

from distutils.command.install import install as DistutilsInstall
from distutils.core import setup
import setuptools
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_orig
from pathlib import Path
import sys
import os
from os.path import expanduser


install_path = Path(sys.prefix) / 'lib/python2.7/'
for a in sys.argv:
	if a == '--user':
		install_path = Path( expanduser("~") + '/.local/lib/python2.7/' )
		break
print install_path

def setup_GPGOMEA_Cpp():
	# create dir for make
	os.system('mkdir dist')
	os.system('mkdir dist/Python_Release')
	
	# switch to python makefile
	os.system('mv Makefile Makefile.backup')
	os.system('mv pyMakefile Makefile')
	os.system('cp Makefile-impl-py.mk nbproject/')

	# make
	os.system('make')

	# restore original makefile
	os.system('mv Makefile pyMakefile')
	os.system('mv Makefile.backup Makefile')

	# set the library
	os.system('mv dist/Python_Release/*/gpgomea pyGPGOMEA/gpgomea.so')

class Custom_GPGOMEA_Install(DistutilsInstall):
	def run(self):
		setup_GPGOMEA_Cpp()
		DistutilsInstall.run(self)

setup(
	name='pyGPGOMEA',
	version='1.0',
	description='Python GP-GOMEA',
	author='Marco Virgolin',
	author_email='marco.virgolin@gmail.com',
	url='https://github.com/marcovirgolin/GP-GOMEA',
	cmdclass={'install': Custom_GPGOMEA_Install},
	packages=['pyGPGOMEA'],
	#ext_modules=[Extension('gpgomea', [],
	#	libraries=['boost_program_options', 'boost_python', 'boost_system', 'boost_numpy', 'armadillo'])],
	package_data={'': ['gpgomea.so']},
    include_package_data=True,
	)
