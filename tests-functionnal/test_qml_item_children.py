# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributor: Rafael de Lucena Valle <rafaeldelucena@gmail.com>
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

from base import QmlAppTestCase
from funq.client import FunqClient


class TestQmlItemChilden(QmlAppTestCase):

    def get_children_by_property(self, prop, recursive=False):
        self.funq = FunqClient()
        widget = self.funq.active_widget()
        item = widget.item(id='main')
        children = item.children(recursive=recursive)
        return [child.properties().get(prop)
                for child in children.iter() if prop in child.properties()]

    def test_non_recursive_children(self):
        children = self.get_children_by_property('text')
        self.assertIn('Parent', children)

    def test_recursive_children(self):
        children = self.get_children_by_property('text', recursive=True)
        self.assertIn('Parent', children)
        self.assertIn('Child', children)
        self.assertIn('Grandchild', children)
