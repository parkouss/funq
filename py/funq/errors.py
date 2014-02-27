# -*- coding: utf-8 -*-
"""
Déclaration des erreur définies par la librarie funq.
"""

class FunqError(Exception):
    """
    Exception levée lorsque le serveur libFunq renvoie une erreur.
    les attributs `classname` et `desc` sont stockés dans l'exception
    et correspondent aux attributs de même nom renvoyés par le serveur
    libFunq.
    """
    def __init__(self, classname, desc):
        self.classname = classname
        self.desc = desc
        Exception.__init__(self, u"{classname}: {desc}".format(
                classname=classname,
                desc=desc))

class HooqAliasesInvalidLineError(Exception):
    """
    Exception levée lors d'erreur de parsing du fichier d'alias.
    """
    pass

class HooqAliasesKeyError(KeyError):
    """
    Exception levée lors de doublon d'alias ou d'alias non existant.
    """
    pass
