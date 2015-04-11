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

from nose.tools import assert_equals, assert_true
from funq import screenshoter
import tempfile
import shutil
import os


class FakeFunqClient(object):

    def __init__(self):
        self.screens = []

    def take_screenshot(self, fname, type_):
        self.screens.append(fname)


class ScreenShoterCtx(screenshoter.ScreenShoter):

    def __init__(self):
        screenshoter.ScreenShoter.__init__(self, tempfile.mkdtemp())

    def __enter__(self):
        return self

    def __exit__(self, type, value, tb):
        shutil.rmtree(self.working_folder)


def test_take_one_screenshot():
    funq = FakeFunqClient()
    with ScreenShoterCtx() as ctx:
        ctx.take_screenshot(funq, "hello")
        assert_equals(map(os.path.basename, funq.screens), ["0.png"])
        assert_true("0.png: hello" in open(
            os.path.join(ctx.working_folder, 'images.txt')).read())


def test_take_screenshots():
    funq = FakeFunqClient()
    with ScreenShoterCtx() as ctx:
        ctx.take_screenshot(funq, "hello")

        ctx.take_screenshot(funq, "thisisit")

        assert_equals(map(os.path.basename, funq.screens), ["0.png", "1.png"])
        content = open(os.path.join(ctx.working_folder, 'images.txt')).read()
        assert_true("0.png: hello" in content)
        assert_true("1.png: thisisit" in content)
