#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Ce module définit des classes qui seront instanciées à la suite de
réception de données du serveur de l'application testée.

Les classes de ce module définissent l'API d'utilisation de scletest.
"""

from dexml import fields, Model, ModelMetaclass
from xml.dom import minidom
import logging

LOG = logging.getLogger('scletest.models')

# monkey patch pour permettre à la methode render()
# de fonctionner avec des accents (au moins pour dexml version 0.4.2)

def _render_value(self, val): # pylint: disable=C0111,W0613
    if isinstance(val, unicode):
        return val.encode('utf-8')
    return str(val)
fields.Value.render_value = _render_value

class ScleHooqClientModel(Model):
    """
    Classe de base pour tous les modèles nécessitant une connexion
    au serveur de l'application testée.
    
    Un objet :class:`ScleHooqClient` doit être défini par l'intermédiaire
    de :meth:`parse_and_attach` et sera accessible via la méthode
    :meth:`client`.
    """
    @classmethod
    def parse_and_attach(cls, scle_hooq_client, data):
        """
        Appel de la méthode de classe parse et attache les clients
        via un appel de :meth:`_attach_client`.
        """
        obj = cls.parse(data)
        obj._attach_client(scle_hooq_client) # pylint: disable=W0212
        return obj
    
    def __init__(self, *args, **kwargs):
        Model.__init__(self, *args, **kwargs)
        self._scle_hooq_client = None
    
    def client(self):
        """
        Retourne l'instance attachée du client
        :class:`scletest.sclehooq.ScleHooqClient`
        """
        return self._scle_hooq_client
    
    def _attach_client(self, scle_hooq_client):
        """
        Associe l'instance de :class:`scletest.sclehooq.ScleHooqClient`
        passée en paramètre à cet objet.
        """
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

class WidgetMetaClass(ModelMetaclass):
    """
    Ce constructeur de classe force le tagname dexml a etre 'Widget' pour
    toute classe créée.
    """
    def __new__(mcs, name, bases, attrs):
        cls = super(WidgetMetaClass, mcs).__new__(mcs, name, bases, attrs)
        cls.meta.tagname = 'Widget'
        return cls

class Widget(ScleHooqClientModel):
    """
    Classe Widget
    Dérivé de la classe dexml.Model pour le parsing XML des dumps
    
    """
    __metaclass__ = WidgetMetaClass
    name = fields.String(attrname="name")
    class_type = fields.String(attrname="class_type")
    qt_class_types = fields.String(attrname="qt_class_types")
    path = fields.String(attrname="path")
    widgets = fields.List("Widget")

    @classmethod
    def parse(cls, xml):
        """
        Cette méthode de classe permet de renvoyer une instance de sous-classe
        de Widget selon la valeur de l'attribut qt_class_types magiquement.
        
        Remarque: le nom de la sous-classe doit être la valeur de l'attribut
        qt_class_types mais sans le 'Q'.
        
        Exemple:
          
          class ComboBox(Widget):
              pass
                
          xml = ('<Widget name="name1" class_type="class1" path="path1"'
                 ' qt_class_types="QComboBox"/>')
          instance = Widget.parse(xml)
          assert_is_instance(instance, ComboBox)
        
        """
        if isinstance(xml, basestring):
            node = minidom.parseString(xml).documentElement
            class_names = [ cn[1:] for cn in node.attributes['qt_class_types'].value.split(',') ]
            cls = Widget
            for class_name in class_names:
                # parcours des classes en commencant par la plus haute dans la hierarchie
                acceptable_cls = ModelMetaclass.instances_by_classname.get(class_name)
                if acceptable_cls:
                    cls = acceptable_cls
                    break
        return super(Widget, cls).parse(xml)

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
        """
        Permet de définir une valeur pour une propriété (Q_PROPERTY)
        du widget.
        
        .. note::
          
          Si type_ est None, le type sera déterminé automatiquement
          selon le type python de l'objet value passé. Par exemple:
          
          - type(value) == str: 'QString'
          - type(value) == int: 'int'
          - type(value) == float: 'double'
          - type(value) == bool: 'bool'
        
        :param name: nom de la propriété
        :param value: valeur de la propriété
        :param type_: chaine représentant le type QT de la propriété.
        """
        LOG.info('set_property(%r, %r) sur %r' % (name, value, self.path))
        if type_ is None:
            type_ = prop_pytype2qtName(type(value))
            value = prop_value2str(value)
        client = self.client()
        client.send_command(client.COMMANDE_SET_PROPERTY
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
        LOG.info('properties() sur %r' % self.path)
        client = self.client()
        data = client.send_command(client.COMMANDE_DUMP_PROPERTIES
                                                    .format(self.path))
        properties = Properties.parse(data)
        if as_py_dict:
            return properties.as_py_dict()
        else:
            return properties

    def click(self):
        """
        Click sur le widget
        """
        LOG.info('click() sur %r' % self.path)
        client = self.client()
        return client.send_command(client.COMMANDE_CLICK_WIDGET
                                                    .format(self.path))
    
    def shortcut(self, sequence):
        """
        Envoie une QKeySequence sur le widget.
        Voir la documentation de QKeySequence::fromString pour
        savoir quoi passer pour l'argument `sequence`.
        """
        LOG.info('shortcut(%r) sur %r' % (sequence, self.path))
        client = self.client()
        client.send_command(client.COMMANDE_SHORTCUT.format(
                                            target=self.path,
                                            keySequence=sequence))

class ComboBox(Widget):
    """
    Ajoute des méthodes spécifiques aux ComboBox.
    """
    def model_items(self):
        client = self.client()
        # création et affichage de QComboBoxListView
        self.click()
        # recuperation de ce widget QComboBoxListView
        internal_qt_name = '::QComboBoxPrivateContainer-1::QComboBoxListView-1'
        combo_edit_view = client.widget(path=self.path + internal_qt_name)
        model_items = combo_edit_view.model_items()
        # on cache la QComboBoxListView
        combo_edit_view.set_property('visible', False)
        return model_items
    
    def set_current_text(self, text):
        if not isinstance(text, basestring):
            raise TypeError('the text parameter must be a string'
                             ' - got %s' % type(text))
        model_items = self.model_items()
        column = self.properties()['modelColumn']
        index = -1
        for item in model_items.items:
            if column == int(item.column) and item.value == text:
                index = int(item.row)
                break
        assert index > -1, ("Le texte `%s` n'est pas dans la combobox `%s`"
                             % (text, self.path))
        self.set_property('currentIndex', index)

class LineEdit(Widget):
    """
    Représentation d'une QLineEdit
    """
    def set_text(self, text):
        if not isinstance(text, basestring):
            raise TypeError('the text parameter must be a string'
                             ' - got %s' % type(text))
        self.set_property('text', text)
    
    def text(self):
        return self.properties()['text']

class SpinBox(Widget):
    """
    Représentation d'un QSpinBox
    """
    def set_value(self, value):
        if not isinstance(value, int):
            raise TypeError('the text parameter must be an int'
                             ' - got %s' % type(value))
        self.set_property('value', value)
    
    def value(self):
        return self.properties()['value']

class DoubleSpinBox(Widget):
    """
    Représentation d'un QDoubleSpinBox
    """
    def set_value(self, value):
        if not isinstance(value, (float, int)):
            raise TypeError('the text parameter must be  a float or'
                             ' an int - got %s' % type(value))
        self.set_property('value', float(value))
    
    def value(self):
        return self.properties()['value']

class AbstractItemView(Widget):
    """
    Ajout de méthodes spécifiques à toutes les instances de QAbstractItemView.
    """
    editor_class_names = ('QLineEdit', 'QComboBox', 'QSpinBox',
                                   'QDoubleSpinBox')
    def model_items(self):
        """
        Retourne une instance de :class:`ModelItems` contenant tous les items
        du modèle associé à la vue.
        """
        LOG.info('model_items() sur %r' % self.path)
        client = self.client()
        data = client.send_command(client.COMMANDE_DUMP_MODEL
                                                    .format(self.path))
        return ModelItems.parse_and_attach(client, data)

    def model_item(self, row, column=0, path=''):
        """
        Retourne une instance d'un :class:`Item`.
        """
        client = self.client()
        data = client.send_command(client.COMMANDE_GET_ITEM
                     .format(view_target=self.path,
                             item_path=path,
                             row=row,
                             column=column))
        return Item.parse_and_attach(client, data)
    
    def current_editor(self, editor_class_name=None):
        """
        Retourne l'éditeur d'item actuellement ouvert sur cette vue.
        l'item doit être en mode édition, ce qui peut être fait par
        l'appel de :meth:`Item.dclick` ou :meth:`Item.edit`.
        
        Les types d'éditeur gérés sont actuellement les suivants:
        'QLineEdit', 'QComboBox', 'QSpinBox' et 'QDoubleSpinBox'.
        
        :param editor_class_name: chaine représentant le type de
                                  l'éditeur. Si None, tous les types
                                  d'éditeurs sont testés.
        """
        # late import, because scletest.sclehooq also import this module
        from scletest.sclehooq import AckError
        client = self.client()
        qt_path = '::qt_scrollarea_viewport::%s-1'
        if editor_class_name:
            return client.widget(path=self.path
                                  + qt_path % editor_class_name)
        for editor_class_name in self.editor_class_names:
            try:
                return client.widget(path=self.path
                                      + qt_path % editor_class_name)
            except AckError:
                pass
        raise AckError('Unable to find an editor. Possible editors:'
                        ' %s' % repr(self.editor_class_names))
    
    def edit_item(self, item, value, editor_class_name=None):
        """
        Edite un item pour lui positionner la valeur value.
        
        :param item: l'instance de :class:`Item` qui doit être modifiée.
        :param value: la nouvelle valeur de l'item. Doit être du type
                      correspondant à l'éditeur (str pour LineEdit,
                      int pour SpinBox, etc)
        """
        if item.is_checkable():
            # ok, on a a faire a une case a cocher.
            # le probleme c'est qu'il n'y a pas vraiment d'editeur pour
            # cela souvent
            if not isinstance(value, bool):
                raise TypeError("must be a bool")
            if value != item.is_checked():
                item.dclick()
            return
        # mieux vaut effectuer un double click, comme c'est le moyen
        # le plus classique de passer en édition pour un utilisateur
        item.dclick()
        editor = self.current_editor(editor_class_name)
        if isinstance(editor, ComboBox):
            editor.set_current_text(value)
        elif isinstance(editor, LineEdit):
            editor.set_text(value)
        elif isinstance(editor, (SpinBox, DoubleSpinBox)):
            editor.set_value(value)
        else:
            raise TypeError("Don't know how to edit an item with"
                             " an editor of type %s." % type(editor))

class WidgetsTree(ScleHooqClientModel):
    """
    Classe WidgetsTree 
    Utilisée pour parser le dump XML de l'arborescence des widgets
    Dérivé de la classe dexml.Model
    """
    widgets = fields.List(Widget)
    
    def _attach_client(self, scle_hooq_client):
        for w in self.widgets:
            w._attach_client(scle_hooq_client) # pylint: disable=W0212
    
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
        'QString': unicode,
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
    """
    Représente un model item de QT contenu dans un modele QT.
    """
    view_path = fields.String(attrname="view_path")
    row = fields.String(attrname="row")
    column = fields.String(attrname="column")
    value = fields.String(attrname="value")
    path = fields.String(attrname="path", required=False)
    check_state = fields.String(attrname="check_state", required=False)
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
    def is_checkable(self):
        return self.check_state is not None
    
    def is_checked(self):
        return self.check_state == 'checked'
    
    def select(self):
        """
        Sélectionne l'item.
        """
        self._item_action('selectItem')
    
    def click(self):
        """
        Clique sur l'item.
        """
        self._item_action('clickItem')
    
    def dclick(self):
        """
        Effectue un double-click sur l'item.
        """
        self._item_action('dClickItem')
    
    def edit(self):
        """
        Passe l'item en mode editable, en ouvrant l'éditeur associé.
        """
        self._item_action('editItem')

class ModelItems(ScleHooqClientModel):
    name = fields.String(attrname="name")
    class_type = fields.String(attrname="class_type")
    path = fields.String(attrname="path")
    items = fields.List("Item")

    def _attach_client(self, scle_hooq_client):
        for item in self.items:
            item._attach_client(scle_hooq_client)
    
    def item_by_named_path(self, named_path, match_column=0, sep='/', column=0):
        """
        Renvoie l'item (:class:`Item`) correspondant au chemin arborescent
        défini par `named_path` correspondant à la colonne `column` ou
        None si le chemin n'existe pas.
        
        .. note::
          
          Les arguments sont les mêmes que pour :meth:`row_by_named_path`,
          avec l'ajout de `column`.
        
        :param column: la colonne de l'item à récupérer.
        """
        items = self.row_by_named_path(named_path,
                                         match_column=match_column,
                                         sep=sep)
        if items:
            return items[column]

    def row_by_named_path(self, named_path, match_column=0, sep='/'):
        """
        Renvoie la liste de :class:`Item` correspondant à une ligne
        du modèle selon un chemin arborescent défini par le nom
        des items, ou None si le chemin n'existe pas.
        
        .. important::
          
          Penser à utiliser des chaines unicodes pour matcher les éléments
          contenant des accents pour le paramètre `named_path`.
        
        Exemple::
          
          model_items.row_by_named_path([u'TG/CBO/AP (AUT 1)',
                                         u'Paramètres tranche',
                                         u'TG',
                                         u'DANGER'])
        
        :param named_path: le chemin du modelIndex interessant. Peut être
                           défini par une liste de chaine, chacune étant
                           un nom d'item ou par un chaine unique utilisant
                           `sep` comme séparateur.
        :param match_column: colonne sur laquelle on vérifie le nom des
                             items.
        :param sep: Séparateur, utilisé selement si `named_path` est une
                    chaine.
        """
        if isinstance(named_path, (list, tuple)):
            parts = named_path[:]
        else:
            parts = named_path.split(sep)
        item = self
        result = []
        while item and parts:
            next_item = None
            part = parts.pop(0)
            for item_ in item.items:
                if match_column == int(item_.column) and item_.value == part:
                    # on a trouvé l'item que l'on veut
                    # si c'est le dernier, ramassons toutes les colonnes
                    if not parts:
                        row = [ it for it in item.items
                                if it.row == item_.row ]
                        return sorted(row, key=lambda it: it.column)
                    else:
                        next_item = item_
            item = next_item
        return None
