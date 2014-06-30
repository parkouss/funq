from base import AppTestCase

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
