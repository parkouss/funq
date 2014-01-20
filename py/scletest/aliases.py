#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

class HooqAliases(dict):
    """
    Un dict spécialisé pour stocker des alias.
    """
    def __setitem__(self, name, value):
        if name in self:
            raise HooqAliasesKeyError("L'alias `%s` existe deja."
                                       % name)
        try:
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
    def from_file(cls, path):
        """
        Créé une instance de :class:`HooqAliases` depuis un fichier
        texte
        """
        self = cls()
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


