#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
 --------- Documentation du module ---------
Ce fichier fait partie du tutorial de **funq**. Il définit
actuellement une seule fonction de test.
"""

from funq.testcase import FunqTestCase
import time

class MyTestCase(FunqTestCase):
    # recuperation de la config adéquate pour l'application concernée
    app_config_name = 'applitest'

    def test_mon_premier_test(self):
        """
         --------- Documentation de la fonction ---------
        """
        # ne fait rien pendant 3 secondes.
        time.sleep(3)
