#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
 --------- Module's documentation ---------
This file is part of the funq tutorial. It currently defines
one test method.
"""

from funq.testcase import FunqTestCase
import time


class MyTestCase(FunqTestCase):
    # identify the configuration
    __app_config_name__ = 'applitest'

    def test_my_first_test(self):
        """
         --------- Test method documentation ---------
        """
        # do nothing for 3 seconds
        time.sleep(3)
