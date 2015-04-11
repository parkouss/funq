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

from nose.tools import assert_equals, raises
from funq.aliases import HooqAliases, HooqAliasesInvalidLineError,\
    HooqAliasesKeyError
from tempfile import NamedTemporaryFile
import os


class TestAliases:

    def setup(self):
        self.aliases = HooqAliases()

    def test_add_one(self):
        self.aliases['1'] = '2'
        assert_equals(self.aliases['1'], '2')

    @raises(HooqAliasesKeyError)
    def test_no_doublons(self):
        self.aliases['1'] = '2'
        self.aliases['1'] = '2'

    def test_substitution(self):
        self.aliases['a'] = '1'
        self.aliases['b'] = '{a}2'
        assert_equals(self.aliases['b'], '12')

    @raises(HooqAliasesKeyError)
    def test_bad_substitution(self):
        self.aliases['b'] = '{a}2'
        self.aliases['a'] = '1'
        assert_equals(self.aliases['b'], '12')

    @raises(HooqAliasesKeyError)
    def test_alias_inexistant(self):
        self.aliases['b']


class TestAliasesFromFile:

    def _parse(self, data, gkit_data=None, gkit='default'):
        f = NamedTemporaryFile(delete=False)
        path = f.name
        if gkit_data:
            fgkit = NamedTemporaryFile(delete=False)
            pathgkit = fgkit.name
            fgkit.write(gkit_data.encode('utf-8'))
            fgkit.close()
        else:
            pathgkit = None
        try:
            f.write(data.encode('utf-8'))
            f.close()

            aliases = HooqAliases.from_file(path, pathgkit, gkit)
        finally:
            os.unlink(path)
            if pathgkit:
                os.unlink(pathgkit)
        return aliases

    def test_simple_parse(self):
        aliases = self._parse("""
a = 1
b = 2
""")
        assert_equals(aliases, {'a': '1', 'b': '2'})

    def test_parse_with_comment(self):
        aliases = self._parse("""
a = 1
# toto = 1
b = 2
""")
        assert_equals(aliases, {'a': '1', 'b': '2'})

    def test_parse_with_empty_line(self):
        aliases = self._parse("""
a = 1

b = 2
""")
        assert_equals(aliases, {'a': '1', 'b': '2'})

    @raises(HooqAliasesInvalidLineError)
    def test_parse_with_syntax_error(self):
        self._parse("""
a  1
""")

    def test_with_gkit(self):
        gkit_data = """
[default]
MY_DEFINE = 33
"""
        aliases = self._parse("""
a = {MY_DEFINE}::1
b = 2
""", gkit_data)
        assert_equals(aliases, {'MY_DEFINE': '33', 'a': '33::1', 'b': '2'})

    def test_with_gkit_interpolation(self):
        gkit_data = """
[default]
MY_DEFINE = 33
OTHER_DEFINE = {MY_DEFINE}::66
"""
        aliases = self._parse("""
a = {MY_DEFINE}::1
b = {OTHER_DEFINE}
""", gkit_data)
        assert_equals(aliases, {'MY_DEFINE': '33', 'a': '33::1',
                                'OTHER_DEFINE': '33::66', 'b': '33::66'})

    def test_with_gkit_custom(self):
        gkit_data = """
[default]
MY_DEFINE = 33
[kde]
MY_DEFINE = 66
"""
        aliases_data = """
a = {MY_DEFINE}::1
b = 2
"""
        aliases = self._parse(aliases_data, gkit_data)
        assert_equals(aliases, {'MY_DEFINE': '33', 'a': '33::1', 'b': '2'})

        aliases = self._parse(aliases_data, gkit_data, 'kde')
        assert_equals(aliases, {'MY_DEFINE': '66', 'a': '66::1', 'b': '2'})
