from funq.testcase import FunqTestCase

class AppTestCase(FunqTestCase):
    __app_config_name__ = 'app_test'
    
    def start_dialog(self,btn_name):
        btn = self.funq.widget(path='mainWindow::QWidget::' + btn_name)
        btn.click()

    def get_status_text(self):
        return self.funq.widget(path='mainWindow::statusBar::QLabel').properties()['text']
