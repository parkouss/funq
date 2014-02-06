#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Ce module définit l'implémentation des alias pour permettre de donner
des noms à un path de widget.
"""

from ConfigParser import ConfigParser
import collections

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

def set_gkit_aliases(hooqaliases, gkit_file, gkit):
    cfg = ConfigParser(dict_type=collections.OrderedDict)
    cfg.read([gkit_file])
    for optname in cfg.options(gkit):
        optname = optname.upper()
        hooqaliases[optname] = cfg.get(gkit, optname)

class HooqAliases(dict):
    """
    Un dict spécialisé pour stocker des alias.
    """
    def __setitem__(self, name, value):
        if name in self:
            raise HooqAliasesKeyError("L'alias `%s` existe deja."
                                       % name)
        try:
            # pylint: disable=W0142
            value = value.format(**self)
        except KeyError, msg:
            raise HooqAliasesKeyError("Substitution impossible dans"
                                       " l'alias %s: %s" % (name, msg))
        dict.__setitem__(self, name, value)
    
    def __getitem__(self, name):
        try:
            return dict.__getitem__(self, name)
        except KeyError:
            raise HooqAliasesKeyError("L'alias `%s` n'existe pas."
                                       % name)
    
    @classmethod
    def from_file(cls, path, gkit_file=None, gkit='default'):
        """
        Créé une instance de :class:`HooqAliases` depuis un fichier
        texte
        """
        self = cls()
        if gkit_file:
            set_gkit_aliases(self, gkit_file, gkit)
        with open(path) as f:
            num_line = 0
            for line in f:
                line = line.strip()
                num_line += 1
                if not line or line.startswith('#'):
                    continue
                else:
                    try:
                        key, value = line.split('=', 1)
                    except ValueError:
                        raise HooqAliasesInvalidLineError(
                                "Le fichiers d'alias `%s` contient une"
                                " erreur de syntaxe ligne %d."
                                % (path, num_line))
                self[key.strip()] = value.strip()
        return self


