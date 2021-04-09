#!/usr/bin/env python

from setuptools import setup

setup(
	name='pyGPGOMEA',
	version='1.3.0',
	description='Python GP-GOMEA',
	author='Marco Virgolin',
	author_email='marco.virgolin@gmail.com',
	url='https://github.com/marcovirgolin/GP-GOMEA',
	packages=['pyGPGOMEA'],
	package_data={'pyGPGOMEA': ['gpgomea.so']},
    include_package_data=True,
    install_requires=['scikit-learn==0.23.2'],
	)

