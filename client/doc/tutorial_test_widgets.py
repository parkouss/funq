#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This file is part of the funq tutorial. It briefly shows widget
interaction.
"""

from funq.testcase import FunqTestCase
from funq.client import FunqError

class TestCase2(FunqTestCase):
    # identify the configuration
    __app_config_name__ = 'applitest'

    def test_libelle_btn_test(self):
        """
        Test the test button libelle
        """
        # use  the "btnTest" alias
        btn_test = self.funq.widget('btnTest')
        properties = btn_test.properties()
        
        self.assertEquals(properties['text'], 'Test')

    def test_open_dialog(self):
        """
        Test that a dialog is open when user click on the test button.
        """
        def dialog1_is_closed():
            """helper function"""
            try:
                self.funq.widget('dialog1_btn_ok')
            except FunqError:
                return True
            else:
                return False
        
        self.assertTrue(dialog1_is_closed(),
                        'The widget `dialog1_btn_ok` is opened, but it must not')
        
        btn_test = self.funq.widget('btnTest')
        btn_test.click()
        
        self.assertFalse(dialog1_is_closed(),
                         'The widget `dialog1_btn_ok` est closed, but i must not')
        
        btn_dlg = self.funq.widget('dialog1_btn_ok')
        btn_dlg.click()
        
        self.assertTrue(dialog1_is_closed(),
                        'The widget `dialog1_btn_ok` is opened, but it must not')
