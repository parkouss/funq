from setuptools import setup
import platform

setup(
    name='funq_server',
    packages=['funq_server'],
    entry_points = {
        'console_scripts': [
            'funq = funq_server.runner:main'
        ]
    }
)
