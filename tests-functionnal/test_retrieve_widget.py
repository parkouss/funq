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
from funq.testcase import parameterized

from funq.models import Widget, ComboBox, HeaderView
from funq.client import FunqClient
from funq.errors import HooqAliasesKeyError, FunqError


class TestRetrieve(AppTestCase):

    @parameterized('path', path='mainWindow::RetrieveWidget::QLabel')
    @parameterized('alias', alias='lbl_retrieve')
    def test_widget(self, alias=None, path=None):
        self.start_dialog('retrieve')
        lbl = self.funq.widget(alias=alias, path=path)

        self.assertIsInstance(lbl, Widget)
        self.assertIsInstance(lbl.client, FunqClient)
        self.assertEquals(lbl.path, 'mainWindow::RetrieveWidget::QLabel')
        self.assertEquals(lbl.classes, [u'QLabel', u'QFrame', u'QWidget', u'QObject'])
        self.assertTrue(lbl.oid)

    def test_widget_alias_unknow(self):
        with self.assertRaises(HooqAliasesKeyError):
            lbl = self.funq.widget('toto')

    def test_widget_path_unavailable(self):
        with self.assertRaisesRegexp(FunqError, "InvalidWidgetPath"):
            lbl = self.funq.widget(path='toto', timeout=0.1)

    def test_widget_subclass(self):
        self.start_dialog('retrieve')
        combo = self.funq.widget(path='mainWindow::RetrieveWidget::QComboBox')
        self.assertIsInstance(combo, ComboBox)

    def test_widget_property(self):
        self.start_dialog('retrieve')
        lbl = self.funq.widget('lbl_retrieve')

        self.assertEquals(lbl.properties()['text'], 'hello')

    def test_widget_set_property(self):
        self.start_dialog('retrieve')
        lbl = self.funq.widget('lbl_retrieve')
        lbl.set_property('text', 'hello2')
        self.assertEquals(lbl.properties()['text'], 'hello2')

    def test_widget_set_properties(self):
        self.start_dialog('retrieve')
        lbl = self.funq.widget('lbl_retrieve')
        lbl.set_properties(text='hello2', wordWrap=True)
        self.assertEquals(lbl.properties()['text'], 'hello2')
        self.assertEquals(lbl.properties()['wordWrap'], True)

    def test_widget_active(self):
        self.start_dialog('retrieve')
        self.funq.widget(path='mainWindow::RetrieveWidget')  # wait for the dialog to be shown
        active = self.funq.active_widget('modal')
        self.assertEquals(active.path, 'mainWindow::RetrieveWidget')

    @parameterized('V', 'V', ['R1', 'R2'])
    @parameterized('H', 'H', ['C1', 'C2', 'C3'])
    def test_headertexts(self, orientation, texts):
        self.start_dialog('table')
        header = self.funq.widget(path='mainWindow::TableDialog::QTableWidget::' + orientation)
        self.assertEquals(header.header_texts(), texts)

    def test_header_from_table(self):
        self.start_dialog('table')
        table = self.funq.widget(path='mainWindow::TableDialog::QTableWidget')
        self.assertIsInstance(table.horizontal_header(), HeaderView)
        self.assertIsInstance(table.vertical_header(), HeaderView)
