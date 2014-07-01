from base import AppTestCase
from funq.testcase import parameterized

from funq.models import Widget, ComboBox
from funq.client import FunqClient
from funq.errors import HooqAliasesKeyError, FunqError

class TestRetrieve(AppTestCase):
    
    @parameterized('path', path='mainWindow::RetrieveWidget::QLabel')
    @parameterized('alias', alias='lbl_retrieve')
    def test_widget(self, alias=None, path=None):
        self.start_dialog('retrieve')
        lbl = self.funq.widget(alias=alias, path=path)

        self.assertIsInstance(lbl, Widget)
        self.assertIsInstance(lbl.client, FunqClient)
        self.assertEquals(lbl.path, 'mainWindow::RetrieveWidget::QLabel')
        self.assertEquals(lbl.classes, [u'QLabel', u'QFrame', u'QWidget', u'QObject'])
        self.assertTrue(lbl.oid)
    
    def test_widget_alias_unknow(self):
        with self.assertRaises(HooqAliasesKeyError):
            lbl = self.funq.widget('toto')
    
    def test_widget_path_unavailable(self):
        with self.assertRaisesRegexp(FunqError, "InvalidWidgetPath"):
            lbl = self.funq.widget(path='toto', timeout=0.1)
    
    def test_widget_subclass(self):
        self.start_dialog('retrieve')
        combo = self.funq.widget(path='mainWindow::RetrieveWidget::QComboBox')
        self.assertIsInstance(combo, ComboBox)
    
    def test_widget_property(self):
        self.start_dialog('retrieve')
        lbl = self.funq.widget('lbl_retrieve')
        
        self.assertEquals(lbl.properties()['text'], 'hello')
    
    def test_widget_set_property(self):
        self.start_dialog('retrieve')
        lbl = self.funq.widget('lbl_retrieve')
        lbl.set_property('text', 'hello2')
        self.assertEquals(lbl.properties()['text'], 'hello2')
    
    def test_widget_set_properties(self):
        self.start_dialog('retrieve')
        lbl = self.funq.widget('lbl_retrieve')
        lbl.set_properties(text='hello2', wordWrap=True)
        self.assertEquals(lbl.properties()['text'], 'hello2')
        self.assertEquals(lbl.properties()['wordWrap'], True)
    
    def test_widget_active(self):
        self.start_dialog('retrieve')
        self.funq.widget(path='mainWindow::RetrieveWidget') # wait for the dialog to be shown
        active = self.funq.active_widget('modal')
        self.assertEquals(active.path, 'mainWindow::RetrieveWidget')
        
