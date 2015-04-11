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
from funq import noseplugin, tools
from optparse import OptionParser
import tempfile


def configured_plugin(argv=[], env={}):
    plugin = noseplugin.FunqPlugin()
    plugin.enabled = True
    parser = OptionParser()

    plugin.options(parser)
    options = parser.parse_args(argv)[0]
    plugin.configure(options, None)
    return plugin


class FakeTest(object):

    def __init__(self, test_id):
        self.test_id = test_id

    def id(self):
        return self.test_id


def test_simple_configure():
    tools.SNOOZE_FACTOR = 1.0

    funqconf = tempfile.NamedTemporaryFile()
    plugin = configured_plugin(["--funq-conf", funqconf.name])

    assert_equals(1.0, tools.SNOOZE_FACTOR)
    assert_true(plugin.screenshoter is not None)
    assert_equals(plugin.trace_tests, None)


def test_snooze_factor_configure():
    tools.SNOOZE_FACTOR = 1.0

    funqconf = tempfile.NamedTemporaryFile()
    configured_plugin(
        ["--funq-conf", funqconf.name, '--funq-snooze-factor', '3.2']
    )

    assert_equals(3.2, tools.SNOOZE_FACTOR)


def test_before_after():
    noseplugin.FunqPlugin._current_test_name = None

    funqconf = tempfile.NamedTemporaryFile()
    plugin = configured_plugin(["--funq-conf", funqconf.name])

    test = FakeTest("id of test")

    plugin.beforeTest(test)

    plugin.afterTest(test)
