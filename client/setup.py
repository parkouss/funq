# -*- coding: utf-8 -*-
from setuptools import find_packages, setup
import os
import re
import sys

if sys.version_info < (3, 5):
    sys.exit("Python version must be >= 3.5")


def read(*paths):
    this_dir = os.path.dirname(os.path.realpath(__file__))
    content = open(os.path.join(this_dir, *paths), "rb").read()
    return content.decode("utf-8")


version = re.search("__version__ = '(.+)'", read('funq/__init__.py')).group(1)

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
)
