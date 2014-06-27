from funq.testcase import FunqTestCase

class AppTestCase(FunqTestCase):
    app_config_name = 'app_test'
    btn_name = ''
    
    def setUp(self):
        FunqTestCase.setUp(self)
        btn = self.funq.widget(path='mainWindow::QWidget::' + self.btn_name)
        btn .click()

    def get_status_text(self):
        return self.funq.widget(path='mainWindow::statusBar::QLabel').properties()['text']

class TestClick(AppTestCase):
    btn_name = 'click'
    
    def test_simple_click(self):
        btn = self.funq.widget(path='mainWindow::ClickDialog::QPushButton')
        btn.click()
        self.assertEquals(self.get_status_text(), 'clicked !')
