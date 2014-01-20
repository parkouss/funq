#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Ce fichier fait partie du tutorial de **scletest**. Il montre brièvement
l'intéraction avec des widget.
"""

from nose.tools import assert_equals
from scletest.noseplugin import config
from scletest.sclehooq import AckError

# recuperation de la config adéquate pour l'application concernée
cfg = config('applitest')

@cfg.with_hooq
def test_libelle_btn_test(hooq):
    """
    Teste le libellé du bouton de test.
    """
    # utilisation de l'alias "btnTest"
    btn_test = hooq.widget('btnTest')
    properties = btn_test.properties()
    
    assert_equals(properties['text'], 'Test')

@cfg.with_hooq
def test_open_dialog(hooq):
    """
    Teste l'ouverture de la boite de dialogue
    lors de click sur le bouton test.
    """
    def dialog1_is_closed():
        """fonction helper"""
        try:
            hooq.widget('dialog1_btn_ok')
        except AckError:
            return True
        else:
            return False
    
    assert dialog1_is_closed(), ('Le widget `dialog1_btn_ok` est'
                                  'ouvert, il ne devrait pas.')
    
    btn_test = hooq.widget('btnTest')
    btn_test.click()
    
    assert not dialog1_is_closed(), ('Le widget `dialog1_btn_ok` est'
                                      'fermé, il ne devrait pas.')
    
    btn_dlg = hooq.widget('dialog1_btn_ok')
    btn_dlg.click()
    
    assert dialog1_is_closed(), ('Le widget `dialog1_btn_ok` est'
                                  'ouvert, il ne devrait pas.')
