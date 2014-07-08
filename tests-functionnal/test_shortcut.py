from base import AppTestCase
from funq.testcase import parameterized

class TestShortcut(AppTestCase):
    
    @parameterized('F2', 'F2')
    @parameterized('DOWN', 'DOWN')
    @parameterized('ENTER', 'ENTER')
    @parameterized("CTRL+C", "CTRL+C")
    def test_shortcut(self, sequence):
        self.start_dialog('shortcut')
        dlg = self.funq.active_widget('modal')
        dlg.shortcut(sequence)

        self.assertEquals(self.get_status_text(), "Shortcut: " + sequence)
    
    @parameterized("CTRL+C,F2", ["CTRL+C", "F2"])
    @parameterized("CTRL+C,F2,ENTER", ["CTRL+C", "F2", "ENTER"])
    @parameterized("CTRL+C,CTRL+C", ["CTRL+C", "CTRL+C"])
    def test_multi_shortcut(self, sequences):
        self.start_dialog('shortcut')
        dlg = self.funq.active_widget('modal')
        for sequence in sequences:
            dlg.shortcut(sequence)

            self.assertEquals(self.get_status_text(), "Shortcut: " + sequence)

    def test_on_push_button(self):
        self.start_dialog('click')
        btn = self.funq.widget(path='mainWindow::ClickDialog::QPushButton')
        btn.shortcut("ENTER")
        self.assertEquals(self.get_status_text(), 'clicked !')
