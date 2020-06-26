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


class TestClick(AppTestCase):

    def test_simple_click(self):
        self.start_dialog('click')
        btn = self.funq.widget(path='mainWindow::ClickDialog::QPushButton')
        btn.click()
        self.assertEqual(self.get_status_text(), 'clicked !')

    def test_right_click(self):
        self.start_dialog('widgetclick')
        btn = self.funq.widget(path='mainWindow::WidgetClickDialog')
        btn.click(btn='right')
        self.assertEqual(self.get_status_text(), 'right clicked !')

    def test_middle_click(self):
        self.start_dialog('widgetclick')
        btn = self.funq.widget(path='mainWindow::WidgetClickDialog')
        btn.click(btn='middle')
        self.assertEqual(self.get_status_text(), 'middle clicked !')

    def test_double_click(self):
        self.start_dialog('doubleclick')
        btn = self.funq.widget(path='mainWindow::DoubleClickDialog')
        btn.dclick()
        self.assertEqual(self.get_status_text(), 'double clicked !')

    @parameterized('sometext', 'Hello this is me !')
    @parameterized('someothertext', 'AAAA BBBBBBBBBBBBBBBBBB CCCCCCCCCCCCCCCCCCCC')
    def test_key_click(self, text):
        self.start_dialog('keyclick')
        line = self.funq.widget(path='mainWindow::KeyClickDialog::QLineEdit')
        line.keyclick(text)
        self.assertEqual(self.get_status_text(), text)

    @parameterized('R1', 'H', 0, 0)
    @parameterized('R1_by_name', 'H', 'C1', 0)
    @parameterized('V2', 'V', 1, 1)
    def test_click_header(self, orientation, index_or_name, result_index):
        self.start_dialog('table')
        header = self.funq.widget(path='mainWindow::TableDialog::QTableWidget::' + orientation)
        header.header_click(index_or_name)
        self.assertEqual(
            self.get_status_text(),
            orientation + " Header clicked: " + str(result_index)
        )
