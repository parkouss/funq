from __future__ import print_function, division, absolute_import, unicode_literals

import os
from nose.loader import TestLoader
from nose.suite import LazySuite


def create_test_suite():
    this_dir = os.path.dirname(os.path.abspath(__file__))

    return LazySuite(TestLoader().loadTestsFromDir(this_dir))
