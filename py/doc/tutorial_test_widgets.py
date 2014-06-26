#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Ce fichier fait partie du tutorial de **funq**. Il montre brièvement
l'intéraction avec des widget.
"""

from funq.testcase import FunqTestCase
from funq.client import FunqError

class TestCase2(FunqTestCase):
    # recuperation de la config adéquate pour l'application concernée
    app_config_name = 'applitest'

    def test_libelle_btn_test(self):
        """
        Teste le libellé du bouton de test.
        """
        # utilisation de l'alias "btnTest"
        btn_test = self.funq.widget('btnTest')
        properties = btn_test.properties()
        
        self.assertEquals(properties['text'], 'Test')

    def test_open_dialog(self):
        """
        Teste l'ouverture de la boite de dialogue
        lors de click sur le bouton test.
        """
        def dialog1_is_closed():
            """fonction helper"""
            try:
                self.funq.widget('dialog1_btn_ok')
            except FunqError:
                return True
            else:
                return False
        
        self.assertTrue(dialog1_is_closed(),
                        'Le widget `dialog1_btn_ok` est ouvert, il ne devrait pas.')
        
        btn_test = self.funq.widget('btnTest')
        btn_test.click()
        
        self.assertFalse(dialog1_is_closed(),
                         'Le widget `dialog1_btn_ok` est fermé, il ne devrait pas.')
        
        btn_dlg = self.funq.widget('dialog1_btn_ok')
        btn_dlg.click()
        
        self.assertTrue(dialog1_is_closed(),
                        'Le widget `dialog1_btn_ok` est ouvert, il ne devrait pas.')
