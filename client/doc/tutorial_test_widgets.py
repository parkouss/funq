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
        self.funq.widget('btnTest').click()
        dlg_label = self.funq.widget('dialog1_label')

        self.assertEquals(dlg_label.properties()['text'], "Button clicked")

    def test_tableview_content(self):
        """
        Test the data in tableview.
        """
        view = self.funq.widget('tableview')
        items = list(view.model_items().iter())
        self.assertEquals(len(items), 16)

        for item in items:
            text = "row {r}, column {c}".format(r=item.row,
                                                c=item.column)
            self.assertEquals(item.value, text)

    def test_some_treeview_content(self):
        """
        test some data in the treeview
        """
        model = self.funq.widget('treeview').model_items()

        item = model.item_by_named_path([u"item 1", u"item 1-2"])
        parent_item = model.item_by_named_path([u"item 1"])

        self.assertEquals(item.value, u"item 1-2")
        self.assertIn(item, parent_item.items)
