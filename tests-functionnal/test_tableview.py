# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributor: Julien Pagès <j.parkouss@gmail.com>
#
# This software is a computer program whose purpose is to test graphical
# applications written with the QT framework (http://qt.digia.com/).
#
# This software is governed by the CeCILL v2.1 license under French law and
# abiding by the rules of distribution of free software.  You can  use,
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL v2.1 license and that you accept its terms.

from base import AppTestCase


class TestTableView(AppTestCase):

    def test_model_items_count(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        self.assertEquals(len(items.items), 6)

    def test_model_items_value(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            text = str(item.column) + '.' + str(item.row)
            self.assertEquals(item.value, text)

    def test_model_items_checkable(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            self.assertEquals(item.is_checkable(), False)

    def test_model_items_checked(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            self.assertEquals(item.is_checked(), False)

    def test_model_item_select(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            table.select_item(item)
            # Currently there is no way to automatically verify if the item is
            # really selected, but at least we can catch runtime errors, and
            # you can uncomment following line to check it manually:
            #import time; time.sleep(0.5)

    def test_model_item_edit(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            table.edit_item(item)
            # Currently there is no way to automatically verify if the item is
            # really in edit mode, but at least we can catch runtime errors,
            # and you can uncomment following line to check it manually:
            #import time; time.sleep(0.5)

    def test_model_item_click(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            table.click_item(item)
            # Currently there is no way to automatically verify if the item is
            # really clicked, but at least we can catch runtime errors,
            # and you can uncomment following line to check it manually:
            #import time; time.sleep(0.5)

    def test_model_item_dclick(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        items = table.model().items()
        for item in items.iter():
            table.dclick_item(item)
            # Currently there is no way to automatically verify if the item is
            # really clicked, but at least we can catch runtime errors,
            # and you can uncomment following line to check it manually:
            #import time; time.sleep(0.5)
