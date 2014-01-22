
from nose.tools import *
from scletest import models

def test_sclehooq_client():
    client = object()
    
    model = models.ScleHooqClientModel.parse_and_attach(client, '<ScleHooqClientModel/>')
    
    assert_equals(model.client(), client)

class TestWidget:
    def setup(self):
        self.widget = models.Widget(name="name1", class_type="class1", path="path1", qt_class_type="QWidget")
        self.widget.widgets.append(models.Widget(name="name2", class_type="class2", path="path2", qt_class_type="QWidget"))
        self.widget.widgets[0].widgets.append(models.Widget(name="name3", class_type="class3", path="path3", qt_class_type="QWidget"))
    
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
                
        xml = '<Widget name="name1" class_type="class1" path="path1" qt_class_type="QMyWidget"/>'
        instance = models.Widget.parse(xml)
        assert_is_instance(instance, MyWidget)
