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


class TestWidget(AppTestCase):

    def test_map_position_to_from_global(self):
        btn = self.funq.widget(path='mainWindow::QWidget::click')
        local_pos = (10, 20)
        global_pos = btn.map_position_to(*local_pos, parent=None)
        self.assertNotEquals(global_pos, local_pos)
        local_pos_2 = btn.map_position_from(*global_pos, parent=None)
        self.assertEquals(local_pos_2, local_pos)

    def test_map_position_from_to_global(self):
        btn = self.funq.widget(path='mainWindow::QWidget::click')
        global_pos = (100, 200)
        local_pos = btn.map_position_from(*global_pos, parent=None)
        self.assertNotEquals(local_pos, global_pos)
        global_pos_2 = btn.map_position_to(*local_pos, parent=None)
        self.assertEquals(global_pos_2, global_pos)

    def test_map_position_to_from_parent(self):
        win = self.funq.widget(path='mainWindow')
        btn = self.funq.widget(path='mainWindow::QWidget::click')
        local_pos = (10, 20)
        parent_pos = btn.map_position_to(*local_pos, parent=win)
        self.assertNotEquals(parent_pos, local_pos)
        local_pos_2 = btn.map_position_from(*parent_pos, parent=win)
        self.assertEquals(local_pos_2, local_pos)

    def test_map_position_from_to_parent(self):
        win = self.funq.widget(path='mainWindow')
        btn = self.funq.widget(path='mainWindow::QWidget::click')
        parent_pos = (100, 200)
        local_pos = btn.map_position_from(*parent_pos, parent=win)
        self.assertNotEquals(local_pos, parent_pos)
        parent_pos_2 = btn.map_position_to(*local_pos, parent=win)
        self.assertEquals(parent_pos_2, parent_pos)
