# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributors: https://github.com/parkouss/funq/graphs/contributors
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
from funq.errors import FunqError


class TestInjection(AppTestCase):

    def setUp(self):
        # Show a blocking message box *before* Qt's main event loop is entered!
        self.__app_config__.args = ['--show-message-box-at-startup']

        def restore_args():
            self.__app_config__.args = []
        self.addCleanup(restore_args)
        super(TestInjection, self).setUp()

    def test_close_blocking_msgbox_at_startup(self):
        """
        Test if the injection also works if Qt's main event loop was not
        entered yet because of a blocking message box.
        """
        with self.assertRaises(FunqError):
            # the main window must NOT be available yet!
            self.funq.widget(path='mainWindow::QWidget', timeout=2.0)
        # close the blocking message box
        btn = self.funq.widget(path='QMessageBox::qt_msgbox_buttonbox::QPushButton')
        btn.click()
        # now the main window should be visible
        window = self.funq.widget(path='mainWindow::QWidget')
        self.assertEquals(window.properties()['visible'], True)
