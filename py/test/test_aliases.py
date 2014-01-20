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
    def _parse(self, data):
        f = NamedTemporaryFile(delete=False)
        path = f.name
        try:
            f.write(data)
            f.close()
            
            aliases = HooqAliases.from_file(path)
        finally:
            os.unlink(path)
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
