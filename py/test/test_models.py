
from nose.tools import *
from scletest import models

def test_sclehooq_client():
    client = object()
    
    model = models.ScleHooqClientModel.parse_and_attach(client, '<ScleHooqClientModel/>')
    
    assert_equals(model.client(), client)

class TestWidget:
    def setup(self):
        self.widget = models.Widget(name="name1", class_type="class1", path="path1", qt_class_types="QWidget")
        self.widget.widgets.append(models.Widget(name="name2", class_type="class2", path="path2", qt_class_types="QWidget"))
        self.widget.widgets[0].widgets.append(models.Widget(name="name3", class_type="class3", path="path3", qt_class_types="QWidget"))
    
    def test_parse_and_attach(self):
        client = object()
        data = self.widget.render()
        new_widget = models.Widget.parse_and_attach(client, data)
        
        assert_equals(new_widget.client(), client)
        assert_equals(new_widget.widgets[0].client(), client)
        assert_equals(new_widget.widgets[0].widgets[0].client(), client)

    def test_iter_widgets(self):
        widgets = list(self.widget.iter_widgets())
        names = sorted([w.name for w in widgets])
        assert_equals(names, ['name1', 'name2', 'name3'])
    
    def test_find_widget(self):
        assert_equals(self.widget.find_widget('path2'),
                      self.widget.widgets[0])

    def test_properties(self):
        class FakeClient:
            COMMANDE_DUMP_PROPERTIES = '{0}'
            def send_command(*a):
                props = models.Properties(properties=[models.Property(name='name1', property_type='int', value='2')])
                return props.render()

        client = FakeClient()
        data = self.widget.render()
        new_widget = models.Widget.parse_and_attach(client, data)
        
        props = new_widget.properties(as_py_dict=False)
        
        assert_is_instance(props, models.Properties)
        assert_equals(len(props.properties), 1)
        assert_equals(props.properties[0].name, 'name1')
    
    def test_properties_as_py_type(self):
        class FakeClient:
            COMMANDE_DUMP_PROPERTIES = '{0}'
            def send_command(*a):
                props = models.Properties(properties=[
                            models.Property(name='name1', property_type='int', value='2'),
                            models.Property(name='name2', property_type='bool', value='false'),
                            models.Property(name='name3', property_type='bool', value='true'),
                            models.Property(name='name4', property_type='QString', value='toto'),
                            models.Property(name='name5', property_type='UNKNOW', value='toto'),
                ])
                return props.render()

        client = FakeClient()
        data = self.widget.render()
        new_widget = models.Widget.parse_and_attach(client, data)
        props = new_widget.properties()
        
        assert_is_instance(props, dict)
        assert_equals(props, {'name1':2, 'name2': False, 'name3': True, 'name4': 'toto', 'name5': 'toto'})

class TestWidgetInheritance:
    def test_inheritance(self):
        class MyWidget(models.Widget):
            pass
                
        xml = '<Widget name="name1" class_type="class1" path="path1" qt_class_types="QMyWidget"/>'
        instance = models.Widget.parse(xml)
        assert_is_instance(instance, MyWidget)
    
    def test_multi_inheritance_priority(self):
        class MyWidget(models.Widget):
            pass
        class SomethingFirst(models.Widget):
            pass
                
        xml = '<Widget name="name1" class_type="class1" path="path1" qt_class_types="QSomethingFirst,QMyWidget"/>'
        instance = models.Widget.parse(xml)
        assert_is_instance(instance, SomethingFirst)
    
    def test_multi_inheritance(self):
        class MyWidget(models.Widget):
            pass
                
        xml = '<Widget name="name1" class_type="class1" path="path1" qt_class_types="QSomethingFirst,QMyWidget"/>'
        instance = models.Widget.parse(xml)
        assert_is_instance(instance, MyWidget)

class TestModelItems:
    def setup(self):
        self.model_items = models.ModelItems(items=[
            models.Item(view_path='path1',
                        row='0',
                        column='0',
                        value='0-0',
                        items=[
                            models.Item(view_path='path1',
                                        row='0',
                                        column='0',
                                        value='0-0',
                                        path='0-0'),
                            models.Item(view_path='path1',
                                        row='0',
                                        column='1',
                                        value='0-1',
                                        path='0-0'),
                            models.Item(view_path='path1',
                                        row='1',
                                        column='0',
                                        value='1-0',
                                        path='0-0'),
                            models.Item(view_path='path1',
                                        row='1',
                                        column='1',
                                        value='1-1',
                                        path='0-0'),
                        ]),
            models.Item(view_path='path1',
                        row='0',
                        column='1',
                        value='0-1',
                        items=[
                            models.Item(view_path='path1',
                                        row='0',
                                        column='0',
                                        value='0-0',
                                        path='0-0'),
                            models.Item(view_path='path1',
                                        row='0',
                                        column='1',
                                        value='0-1',
                                        path='0-0'),
                        ]),
        ])
    
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
