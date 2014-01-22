#!/usr/bin/env python
# -*- coding: utf-8 -*-

from dexml import fields, Model

# monkey patch pour permettre à la methode render()
# de fonctionner avec des accents (au moins pour dexml version 0.4.2)

def _render_value(self, val):
    if isinstance(val, unicode):
        return val.encode('utf-8')
    return str(val)
fields.Value.render_value = _render_value

class ScleHooqClientModel(Model):
    @classmethod
    def parse_and_attach(cls, scle_hooq_client, data):
        obj = cls.parse(data)
        obj._attach_client(scle_hooq_client)
        return obj
    
    def __init__(self, *args, **kwargs):
        Model.__init__(self, *args, **kwargs)
        self._scle_hooq_client = None
    
    def client(self):
        return self._scle_hooq_client
    
    def _attach_client(self, scle_hooq_client):
        self._scle_hooq_client = scle_hooq_client

def prop_pytype2qtName(type_):
    if issubclass(type_, basestring):
        return 'QString'
    elif issubclass(type_, int):
        return 'int'
    elif issubclass(type_, float):
        return 'double'
    elif issubclass(type_, bool):
        return 'bool'

def prop_value2str(value):
    if isinstance(value, bool):
        return 'true' if value else 'false'
    return str(value)

class Widget(ScleHooqClientModel):
    """
    Classe Widget 
    Dérivé de la classe dexml.Model pour le parsing XML des dumps
    
    """
    name = fields.String(attrname="name")
    class_type = fields.String(attrname="class_type")
    path = fields.String(attrname="path")
    widgets = fields.List("Widget")

    def _attach_client(self, scle_hooq_client):
        widgets = [self]
        while widgets:
            w = widgets.pop(0)
            ScleHooqClientModel._attach_client(w, scle_hooq_client)
            widgets.extend(w.widgets)

    def iter_widgets(self):
        widgets = [self]
        while widgets:
            w = widgets.pop(0)
            yield w
            widgets.extend(w.widgets)

    def find_widget(self, path):
        for w in self.iter_widgets():
            if w.path == path:
                return w

    def set_property(self, name, value, type_=None):
        if type_ is None:
            type_ = prop_pytype2qtName(type(value))
            value = prop_value2str(value)
        client = self.client()
        data = client.send_command(client.COMMANDE_SET_PROPERTY
                                    .format(target=self.path,
                                            propName=name,
                                            propValue=value,
                                            propType=type_))
        

    def properties(self, as_py_dict=True):
        """
        Renvoie les propriétés du widget.
        
        :param as_py_dict: Si True (defaut), les propriétés sont
                           transformées en dictionnaire python avec le
                           type des valeurs changé si possible (voir
                           :meth:`Property.py_value`)
        """
        client = self.client()
        data = client.send_command(client.COMMANDE_DUMP_PROPERTIES
                                                    .format(self.path))
        properties = Properties.parse(data)
        if as_py_dict:
            return properties.as_py_dict()
        else:
            return properties

    def model_items(self):
        client = self.client()
        data = client.send_command(client.COMMANDE_DUMP_MODEL
                                                    .format(self.path))
        return ModelItems.parse_and_attach(client, data)

    def model_item(self, row, column=0, path=''):
        client = self.client()
        data = client.send_command(client.COMMANDE_GET_ITEM
                     .format(view_target=self.path,
                             item_path=path,
                             row=row,
                             column=column))
        return Item.parse_and_attach(client, data)

    def click(self):
        """
        Click sur le widget
        """
        client = self.client()
        return client.send_command(client.COMMANDE_CLICK_WIDGET
                                                    .format(self.path))

class WidgetsTree(ScleHooqClientModel):
    """
    Classe WidgetsTree 
    Utilisée pour parser le dump XML de l'arborescence des widgets
    Dérivé de la classe dexml.Model
    """
    widgets = fields.List(Widget)
    
    def _attach_client(self, scle_hooq_client):
        for w in self.widgets:
            w._attach_client(scle_hooq_client)
    
    def iter_widgets(self):
        for w in self.widgets:
            for sw in w.iter_widgets():
                yield sw
    
    def find_widget(self, path):
        for w in self.iter_widgets():
            if w.path == path:
                return w

class Property(Model):
    """
    Classe Property 
    Stockage d'une proprieté associé à un widget
    """
    name = fields.String(attrname="name")
    property_type = fields.String(attrname="type")
    value = fields.String(attrname="value")
    
    py_types = {
        'QString': str,
        'int': int,
        'bool': lambda v: v!='false',
    }
    
    def py_value(self):
        """
        renvoie le type python de la valeur de la propriété
        si possible.
        
        Actuellement, les types %s sont gérés.
        """ % Property.py_types.keys()
        
        return self.py_types.get(self.property_type, str)(self.value)

class Properties(Model):
    """
    Classe Properties 
    Utilisée pour parser le dump XML des propriétés d'un widget
    Dérivé de la classe dexml.Model
    """
    properties = fields.List(Property)
    
    def as_py_dict(self):
        d = {}
        for prop in self.properties:
            d[prop.name] = prop.py_value()
        return d

class Item(ScleHooqClientModel):
    view_path = fields.String(attrname="view_path")
    row = fields.String(attrname="row")
    column = fields.String(attrname="column")
    value = fields.String(attrname="value")
    path = fields.String(attrname="path", required=False)
    items = fields.List("Item")
    
    def _attach_client(self, scle_hooq_client):
        items = [self]
        while items:
            item = items.pop(0)
            ScleHooqClientModel._attach_client(item, scle_hooq_client)
            items.extend(item.items)

    def _item_action(self, action):
        client = self.client()
        data = client.send_command(client.COMMANDE_MODEL_ITEM
                     .format(view_target=self.view_path,
                             item_path=self.path or '',
                             row=self.row,
                             column=self.column,
                             action=action))
    def select(self):
        self._item_action('selectItem')
    
    def click(self):
        self._item_action('clickItem')
    
    def dclick(self):
        self._item_action('dClickItem')
    
    def edit(self):
        self._item_action('editItem')

class ModelItems(ScleHooqClientModel):
    name = fields.String(attrname="name")
    class_type = fields.String(attrname="class_type")
    path = fields.String(attrname="path")
    items = fields.List("Item")

    def _attach_client(self, scle_hooq_client):
        for item in self.items:
            item._attach_client(scle_hooq_client)
    
    def item_by_named_path(self, named_path):
        if isinstance(named_path, (list, tuple)):
            parts = named_path
        else:
            parts = named_path.split('/')
        item = self
        while parts:
            next_item = None
            part = parts.pop(0)
            for item_ in item.items:
                if item_.value == part:
                    next_item = item_
            if not next_item:
                return None
            item = next_item
        return item

