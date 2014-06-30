from base import AppTestCase
from funq.testcase import parameterized

from funq.models import Widget
from funq.client import FunqClient

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
        
