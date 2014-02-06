# -*- coding: utf-8 -*-

"""
Utilitaires.
"""

import time

class TimeOutError(Exception):
    """Levée lors d'un timeout"""

def wait_for(func, timeout, timeout_interval=0.1):
    """
    Apelle une fonction régulièrement jusqu'à ce qu'elle retourne
    True ou que le timeout expire. Si la fonction retourne une instance
    d'exception, celle si sera levée à la fin du timeout. Sinon,
    :class:`TimeOutError` sera levée.
    
    :param func: fonction à appeller
    :param value: valeur attendue de la propriété
    :param timeout: temps d'attente maximal
    :param timeout_interval: temps d'attente entre chaque demande
                                au serveur.
    
    :raises: TimeOutError sur timeout ou l'exception renvoyée par
             la fonction après timeout.
    """
    elapsed = 0.0
    while 1:
        res = func()
        if res is True:
            return True
        if elapsed >= timeout:
            if isinstance(res, Exception):
                raise res
            raise TimeOutError()
        time.sleep(timeout_interval)
        elapsed += timeout_interval
