#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This file is part of the funq tutorial. It briefly shows widget
interaction.
"""

from __future__ import print_function, division, absolute_import, unicode_literals

from funq.testcase import FunqTestCase


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

        self.assertEqual(properties['text'], 'Test')

    def test_open_dialog(self):
        """
        Test that a dialog is open when user click on the test button.
        """
        self.funq.widget('btnTest').click()
        dlg_label = self.funq.widget('dialog1_label')

        self.assertEqual(dlg_label.properties()['text'], "Button clicked")

    def test_tableview_content(self):
        """
        Test the data in tableview.
        """
        view = self.funq.widget('tableview')
        items = list(view.model_items().iter())
        self.assertEqual(len(items), 16)

        for item in items:
            text = "row {r}, column {c}".format(r=item.row,
                                                c=item.column)
            self.assertEqual(item.value, text)

    def test_some_treeview_content(self):
        """
        test some data in the treeview
        """
        model = self.funq.widget('treeview').model_items()

        item = model.item_by_named_path(["item 1", "item 1-2"])
        parent_item = model.item_by_named_path(["item 1"])

        self.assertEqual(item.value, "item 1-2")
        self.assertIn(item, parent_item.items)
