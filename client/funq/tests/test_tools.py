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

from nose.tools import assert_equals, assert_true, raises
from funq import tools
import time
import sys
import os


def test_wait_for():
    def func():
        return True
    assert_true(tools.wait_for(func, 0.0))


@raises(tools.TimeOutError)
def test_wait_for_timeout():
    def func():
        return False
    tools.wait_for(func, 0.0)


@raises(Exception)
def test_wait_for_custom_exc():
    def func():
        return Exception()
    tools.wait_for(func, 0.0)


def test_wait_for_some_time():
    t = time.time()

    def func():
        return t + 0.05 < time.time()
    assert_true(tools.wait_for(func, 0.1))


def test_which():
    assert_equals(sys.executable, tools.which(sys.executable))


def test_which_with_pass():
    path, fname = os.path.split(sys.executable)
    old_env = os.environ
    env = dict(PATH=path)
    os.environ = env
    try:
        assert_equals(sys.executable, tools.which(fname))
    finally:
        os.environ = old_env


def test_apply_snooze_factor():
    tools.SNOOZE_FACTOR = 3.2
    assert_equals(6.4, tools.apply_snooze_factor(2))
    tools.SNOOZE_FACTOR = 1.0


def test_wait_for_some_time_with_snooze_factor():
    tools.SNOOZE_FACTOR = 4.0
    t = time.time()

    def func():
        return t + 0.05 < time.time()
    assert_true(tools.wait_for(func, 0.025))
    tools.SNOOZE_FACTOR = 1.0
