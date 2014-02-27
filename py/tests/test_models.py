
from nose.tools import *
from funq import models

class TestWidgetInheritance:
    def test_inheritance(self):
        class MyWidget(models.Widget):
            CPP_CLASS = 'TOTO'
                
        instance = models.Widget.create(None, {'classes': ['TOTO']})
        assert_is_instance(instance, MyWidget)
    
    def test_multi_inheritance_priority(self):
        class MyWidget(models.Widget):
            CPP_CLASS = 'TOTO'
        class SomethingFirst(models.Widget):
            CPP_CLASS = 'TITI'
                
        instance = models.Widget.create(None, {'classes': ['TITI', 'TOTO']})
        assert_is_instance(instance, SomethingFirst)
    
    def test_multi_inheritance(self):
        class MyWidget(models.Widget):
            CPP_CLASS = 'TOTO'
                
        instance = models.Widget.create(None, {'classes': ['NotDefined', 'TOTO']})
        assert_is_instance(instance, MyWidget)

class TestModelItems:
    def setup(self):
        data = {
            'items': [
                dict(viewid=1, row=0, column=0, value='0-0', items=[
                    dict(viewid=1, row=0, column=0, value='0-0', itempath='0-0'),
                    dict(viewid=1, row=0, column=1, value='0-1', itempath='0-0'),
                    dict(viewid=1, row=1, column=0, value='1-0', itempath='0-0'),
                    dict(viewid=1, row=1, column=1, value='1-1', itempath='0-0'),
                ]),
                dict(viewid=1, row=0, column=1, value='0-1', items=[
                    dict(viewid=1, row=0, column=0, value='0-0', itempath='0-0'),
                    dict(viewid=1, row=0, column=1, value='0-1', itempath='0-0'),
                ])
            ]
        }
        
        self.model_items = models.ModelItems.create(None, data)
    
    def test_row_by_named_path(self):
        items = self.model_items.row_by_named_path(['0-0', '0-0'])
        assert_equal(items, self.model_items.items[0].items[0:2])

    def test_row_by_named_path_match_column(self):
        items = self.model_items.row_by_named_path(['0-1', '0-1'], match_column=1)
        assert_equal(items, self.model_items.items[1].items[0:2])
    
    def test_row_by_named_path_str_path(self):
        items = self.model_items.row_by_named_path('0-0/0-0')
        assert_equal(items, self.model_items.items[0].items[0:2])
    
    def test_row_by_named_path_str_custom_path(self):
        items = self.model_items.row_by_named_path('0-0::0-0', sep='::')
        assert_equal(items, self.model_items.items[0].items[0:2])
    
    def test_row_by_named_path_missing(self):
        items = self.model_items.row_by_named_path('blah/bluh')
        assert_equals(items, None)
    
    def test_item_by_named_path(self):
        item = self.model_items.item_by_named_path(['0-0', '0-0'])
        assert_equals(item, self.model_items.items[0].items[0])
    
    def test_item_by_named_path_column(self):
        item = self.model_items.item_by_named_path(['0-0', '0-0'], column=1)
        assert_equals(item, self.model_items.items[0].items[1])
    
    def test_item_by_named_path_missing(self):
        item = self.model_items.item_by_named_path('blah/bluh')
        assert_equals(item, None)
