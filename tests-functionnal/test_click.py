from base import AppTestCase
from funq.testcase import parameterized

class TestClick(AppTestCase):
    
    def test_simple_click(self):
        self.start_dialog('click')
        btn = self.funq.widget(path='mainWindow::ClickDialog::QPushButton')
        btn.click()
        self.assertEquals(self.get_status_text(), 'clicked !')
    
    def test_double_click(self):
        self.start_dialog('doubleclick')
        btn = self.funq.widget(path='mainWindow::DoubleClickDialog')
        btn.dclick()
        self.assertEquals(self.get_status_text(), 'double clicked !')
    
    @parameterized('sometext', 'Hello this is me !')
    @parameterized('someothertext', 'AAAA BBBBBBBBBBBBBBBBBB CCCCCCCCCCCCCCCCCCCC')
    def test_key_click(self, text):
        self.start_dialog('keyclick')
        line = self.funq.widget(path='mainWindow::KeyClickDialog::QLineEdit')
        line.keyclick(text)
        self.assertEquals(self.get_status_text(), text)
