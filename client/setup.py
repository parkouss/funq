# -*- coding: utf-8 -*-
from setuptools import find_packages, setup
import os
import re
import sys

if sys.version_info < (2, 7):
    sys.exit("Python version must be > 2.7")


def read(*paths):
    this_dir = os.path.dirname(os.path.realpath(__file__))
    return open(os.path.join(this_dir, *paths)).read()


version = re.search("__version__ = '(.+)'", read('funq/__init__.py')).group(1)

# Nose isn't really needed for this mdoule,
# but it's useful for running tests.
install_requires = ['nose', 'six']

setup(
    name="funq",
    author="Julien Pagès",
    author_email="j.parkouss@gmail.com",
    url="https://github.com/parkouss/funq",
    description="write FUNctional tests for Qt applications (client)",
    long_description=read("README"),
    version=version,
    packages=find_packages(),
    zip_safe=False,
    test_suite='funq.tests.create_test_suite',
    install_requires=install_requires,
    package_data={'funq': ['aliases-gkits.conf']},
    entry_points={
        'nose.plugins.0.10': ['funq = funq.noseplugin:FunqPlugin']
    },
)
