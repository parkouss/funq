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

from nose.tools import assert_is_instance, assert_equals
from funq import models


class TestWidgetInheritance:

    def test_inheritance(self):
        class MyWidget(models.Widget):
            CPP_CLASS = 'TOTO'

        instance = models.Widget.create(None, {'classes': ['TOTO']})
        assert_is_instance(instance, MyWidget)

    def test_multi_inheritance_priority(self):
        class MyWidget(models.Widget):
            CPP_CLASS = 'TOTO'

        class SomethingFirst(models.Widget):
            CPP_CLASS = 'TITI'

        instance = models.Widget.create(None, {'classes': ['TITI', 'TOTO']})
        assert_is_instance(instance, SomethingFirst)

    def test_multi_inheritance(self):
        class MyWidget(models.Widget):
            CPP_CLASS = 'TOTO'

        instance = models.Widget.create(
            None, {'classes': ['NotDefined', 'TOTO']})
        assert_is_instance(instance, MyWidget)


class TestModelItems:

    def setup(self):
        data = {
            'items': [
                dict(viewid=1, row=0, column=0, value='0-0', items=[
                    dict(
                        viewid=1, row=0, column=0, value='0-0',
                        itempath='0-0'),
                    dict(
                        viewid=1, row=0, column=1, value='0-1',
                        itempath='0-0'),
                    dict(
                        viewid=1, row=1, column=0, value='1-0',
                        itempath='0-0'),
                    dict(
                        viewid=1, row=1, column=1, value='1-1',
                        itempath='0-0'),
                ]),
                dict(viewid=1, row=0, column=1, value='0-1', items=[
                    dict(
                        viewid=1, row=0, column=0, value='0-0',
                        itempath='0-0'),
                    dict(
                        viewid=1, row=0, column=1, value='0-1',
                        itempath='0-0'),
                ])
            ]
        }

        self.model_items = models.ModelItems.create(None, data)

    def test_row_by_named_path(self):
        items = self.model_items.row_by_named_path(['0-0', '0-0'])
        assert_equals(items, self.model_items.items[0].items[0:2])

    def test_row_by_named_path_match_column(self):
        items = self.model_items.row_by_named_path(
            ['0-1', '0-1'], match_column=1)
        assert_equals(items, self.model_items.items[1].items[0:2])

    def test_row_by_named_path_str_path(self):
        items = self.model_items.row_by_named_path('0-0/0-0')
        assert_equals(items, self.model_items.items[0].items[0:2])

    def test_row_by_named_path_str_custom_path(self):
        items = self.model_items.row_by_named_path('0-0::0-0', sep='::')
        assert_equals(items, self.model_items.items[0].items[0:2])

    def test_row_by_named_path_missing(self):
        items = self.model_items.row_by_named_path('blah/bluh')
        assert_equals(items, None)

    def test_item_by_named_path(self):
        item = self.model_items.item_by_named_path(['0-0', '0-0'])
        assert_equals(item, self.model_items.items[0].items[0])

    def test_item_by_named_path_column(self):
        item = self.model_items.item_by_named_path(['0-0', '0-0'], column=1)
        assert_equals(item, self.model_items.items[0].items[1])

    def test_item_by_named_path_missing(self):
        item = self.model_items.item_by_named_path('blah/bluh')
        assert_equals(item, None)
