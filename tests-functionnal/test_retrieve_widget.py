from base import AppTestCase
from funq.testcase import parameterized

from funq.models import Widget, ComboBox
from funq.client import FunqClient
from funq.errors import HooqAliasesKeyError, FunqError

class TestClick(AppTestCase):
    
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
