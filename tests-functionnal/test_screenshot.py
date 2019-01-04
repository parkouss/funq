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

import io
import os
from base import AppTestCase


# Note: Captured screenshots are saved in the "screenshots" directory,
#       so they can be checked for correctness manually.
if not os.path.exists('screenshots'):
    os.mkdir('screenshots')


class TestScreenshot(AppTestCase):

    def test_take_screenshot(self):
        buffer = io.BytesIO()
        self.funq.take_screenshot(stream=buffer)
        png = buffer.getvalue()
        with open('screenshots/take_screenshot.png', mode='wb') as f:
            f.write(png)
        self.assertGreater(len(png), 0)

    def test_grab_window(self):
        win = self.funq.widget(path='mainWindow')
        png = win.grab()
        with open('screenshots/grab_window.png', mode='wb') as f:
            f.write(png)
        self.assertGreater(len(png), 0)

    def test_grab_dialog(self):
        self.start_dialog('click')
        dialog = self.funq.widget(path='mainWindow::ClickDialog')
        png = dialog.grab()
        with open('screenshots/grab_dialog.png', mode='wb') as f:
            f.write(png)
        self.assertGreater(len(png), 0)

    def test_grab_button(self):
        self.start_dialog('click')
        btn = self.funq.widget(path='mainWindow::ClickDialog::QPushButton')
        png = btn.grab()
        with open('screenshots/grab_button.png', mode='wb') as f:
            f.write(png)
        self.assertGreater(len(png), 0)
