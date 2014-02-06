from nose.tools import *
from scletest.aliases import HooqAliases, HooqAliasesInvalidLineError,\
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
            fgkit.write(gkit_data)
            fgkit.close()
        else:
            pathgkit = None
        try:
            f.write(data)
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
        aliases = self._parse("""
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
