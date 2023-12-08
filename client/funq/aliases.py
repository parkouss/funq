# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributor: Julien Pagès <j.parkouss@gmail.com>
#
# This software is a computer program whose purpose is to test graphical
# applications written with the QT framework (http://qt.digia.com/).
#
# This software is governed by the CeCILL v2.1 license under French law and
# abiding by the rules of distribution of free software.  You can  use,
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL v2.1 license and that you accept its terms.

"""
This module defines aliases implementation to give names instead of
complete widget's paths.
"""

from configparser import ConfigParser
import collections
from funq.errors import HooqAliasesInvalidLineError, HooqAliasesKeyError


def set_gkit_aliases(funqaliases, gkit_file, gkit):
    """
    Define some aliases in *funqaliases* given the config file *gkit_file*
    for the section *gkit*.
    """
    cfg = ConfigParser(dict_type=collections.OrderedDict)
    cfg.read([gkit_file])
    for optname in cfg.options(gkit):
        optname = optname.upper()
        funqaliases[optname] = cfg.get(gkit, optname)


class HooqAliases(dict):

    """
    A specialized dict for aliases.
    """

    def __setitem__(self, name, value):
        if name in self:
            raise HooqAliasesKeyError("The alias `%s` already exists."
                                      % name)
        try:
            # pylint: disable=W0142
            value = value.format(**self)
        except KeyError as msg:
            raise HooqAliasesKeyError("Impossible substitution in"
                                      " the alias %s: %s." % (name, msg))
        dict.__setitem__(self, name, value)

    def __getitem__(self, name):
        try:
            return dict.__getitem__(self, name)
        except KeyError:
            raise HooqAliasesKeyError("The alias `%s` does not exists."
                                      % name)

    @classmethod
    def from_file(cls, path, gkit_file=None, gkit='default'):
        """
        Create an instance of :class:`HooqAliases` from a text file.
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
                            "The alias file `%s` contains a"
                            " syntax error on line %d."
                            % (path, num_line))
                self[key.strip()] = value.strip()
        return self
