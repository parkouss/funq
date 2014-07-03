# -*- coding: utf-8 -*-

"""
Utilitaires.
"""

import os
import platform
import time
from funq.errors import TimeOutError

# permet de modifier le temps d'attente par défaut au niveau global
SNOOZE_FACTOR = 1.0

def apply_snooze_factor(value):
    """Applique le facteur de snooze global"""
    return value * SNOOZE_FACTOR

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
    timeout = apply_snooze_factor(timeout)
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

def is_exe(fpath):
    """Renvoie True si fpath est un fichier exécutable"""
    return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

def _which(program):
    """
    Internal
    """
    if os.path.dirname(program):
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None

def which(program):
    """
    Tente de localiser un executable sur PATH.
    """
    if platform.system() == 'Windows':
        # try with exe suffix first
        pgm = _which(program + '.exe')
        if pgm:
            return pgm
    return _which(program)
