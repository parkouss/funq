from setuptools import find_packages, setup
import os, re

def read(*paths):
	this_dir = os.path.dirname(os.path.realpath(__file__))
	return open(os.path.join(this_dir, *paths)).read()

version = re.search("__version__ = '(.+)'", read('funq/__init__.py')).group(1)

# nose n'est actuellement pas requis pour ce module
# mais tres utile pour cadrer les tests.
install_requires = ['nose']

setup(
	name="funq",
	version=version,
	packages=find_packages(),
    zip_safe=False,
	install_requires=install_requires,
    package_data={'funq': ['aliases-gkits.conf']},
	entry_points = {
        'nose.plugins.0.10': [
            'funq = funq.noseplugin:FunqPlugin'
            ]
        },
)
