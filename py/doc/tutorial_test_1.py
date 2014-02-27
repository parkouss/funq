#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
 --------- Documentation du module ---------
Ce fichier fait partie du tutorial de **funq**. Il définit
actuellement une seule fonction de test.
"""

from funq.noseplugin import config
import time

# recuperation de la config adéquate pour l'application concernée
CFG = config('applitest')

@CFG.with_hooq
def test_mon_premier_test(hooq):
    """
     --------- Documentation de la fonction ---------
    """
    # ne fait rien pendant 3 secondes.
    time.sleep(3)
