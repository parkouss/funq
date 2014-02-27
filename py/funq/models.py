# -*- coding: utf-8 -*-
"""
Déclaration des modèles manipulables avec le framework de test.
"""
from funq.tools import wait_for
import json

class TreeItem(object):
    """
    Représente un item abstrait, contenant d'autre items.
    """
    
    @classmethod
    def create(cls, client, data):
        """
        Permet de créer un TreeItem selon un dictionnaire de données provenant
        d'un décodage json.
        """
        self = cls()
        self.client = client
        for k, v in data.iteritems():
            if k != 'items':
                setattr(self, k, v)
        self.items = [cls.create(client, d) for d in data.get('items', []) ]
        return self

class TreeItems(object):
    """
    classe abstraite pour manipuler des données contenant des :class:`TreeItem`.
    Utilisé pour les modelitem et les graphicsitems.
    """
    ITEM_CLASS = TreeItem
    
    @classmethod
    def create(cls, client, data):
        """
        Permet de créer un modele d'"items" selon un dictionnaire de données
        provenant d'un décodage json.
        """
        self = cls()
        self.client = client
        self.items = [cls.ITEM_CLASS.create(client, v1) for v1 in data['items']]
        return self
    
    def iter(self):
        """
        Itere sur tous les items.
        """
        items = self.items
        while items:
            item = items.pop(0)
            items = item.items + items
            yield item

class WidgetMetaClass(type):
    """
    Permet de stocker un dictionnaire des classes accessibles, pour intégrer
    l'héritage des classes C++ manipulées avec l'héritage python.
    """
    cpp_classes = {}
    def __new__(mcs, name, bases, attrs):
        cls = super(WidgetMetaClass, mcs).__new__(mcs, name, bases, attrs)
        qt_name = getattr(cls, 'CPP_CLASS', None)
        if qt_name:
            mcs.cpp_classes[qt_name] = cls
        return cls

class Widget(object):
    """
    Permet de manipuler un QWidget ou dérivé.
    """
    __metaclass__ = WidgetMetaClass
    
    oid = None
    client = None
    
    @classmethod
    def create(cls, client, data):
        """
        Permet de créer un Widget ou une sous-classe selon un dictionnaire
        de données provenant d'un décodage json.
        """
        # recherche la classe appropriee
        cpp_classes = cls.__metaclass__.cpp_classes
        for cppcls in data['classes']:
            if cppcls in cpp_classes:
                cls = cpp_classes[cppcls]
                break
        
        self = cls()
        for k, v in data.iteritems():
            setattr(self, k, v)
        setattr(self, 'client', client)
        return self

    def properties(self):
        """
        Retourne un dictionnaire de propriétés accessibles pour ce widget
        avec leur valeurs courantes.
        """
        return self.client.send_command('object_properties', oid=self.oid)

    def set_properties(self, **properties):
        """
        Permet de définir des propriétés sur un widget. Exemple::
          
          widget.set_properties(text="Mon beau texte")
        """
        self.client.send_command('object_set_properties',
                                 oid=self.oid,
                                 properties=properties)
    
    def set_property(self, name, value):
        """
        Permet de définir une propriété pour ce widget. Exemple::
          
          widget.set_property('text', "Mon beau texte")
        """
        self.set_properties(**{name: value})

    def wait_for_properties(self, props, timeout=10.0, timeout_interval=0.1):
        """
        Attends que les propriétés prennent les valeurs désirées. Exemple::
          
          self.wait_for_properties({'enabled': True, 'visible': True})
        """
        def check_props():
            properties = self.properties()
            for k, v in props.iteritems():
                if properties.get(k) != v:
                    return False
            return True
        return wait_for(check_props, timeout, timeout_interval)

    def click(self, wait_for_enabled=10.0):
        """
        Click sur le widget. Si wait_for_enabled est > 0 (défaut), on attends
        que le widget soit actif (enabled et visible) avant de cliquer.
        """
        if wait_for_enabled > 0.0:
            self.wait_for_properties({'enabled': True, 'visible': True},
                                     timeout=wait_for_enabled)
        self.client.send_command('widget_click', oid=self.oid)
    
    def dclick(self, wait_for_enabled=10.0):
        """
        Double click sur le widget. Si wait_for_enabled est > 0 (défaut), on
        attends que le widget soit actif (enabled et visible) avant de cliquer.
        """
        if wait_for_enabled > 0.0:
            self.wait_for_properties({'enabled': True, 'visible': True},
                                     timeout=wait_for_enabled)
        self.client.send_command('widget_click',
                                 oid=self.oid,
                                 action='doubleclick')
    
    def keyclick(self, text):
        """
        Simule les évènements keypress et keyrelease pour chaque lettre du texte
        passé sur ce widget. Exemple::
          
          widget.keyclick("mon texte")
        """
        self.client.send_command('widget_keyclick', text=text, oid=self.oid)
    
    def shortcut(self, key_sequence):
        """
        Envoi un raccourci clavier sur ce widget, défini par une séquence de
        texte. Le format de la séquence est défini par QKeySequence::fromString.
        """
        self.client.send_command('shortcut',
                                 keysequence=key_sequence,
                                 oid=self.oid)

class ModelItem(TreeItem):
    """
    Représente un modelitem présent dans un QAbstractModelItem ou dérivé.
    """
    
    viewid = None
    row = None
    column = None
    
    def _action(self, itemaction):
        self.client.send_command('model_item_action',
                                 oid=self.viewid,
                                 itemaction=itemaction,
                                 row=self.row, column=self.column,
                                 itempath=getattr(self, 'itempath', ''))
    
    def select(self):
        """
        Sélectionne l'item.
        """
        self._action("select")
    
    def edit(self):
        """
        Passe l'item en mode édition.
        """
        self._action("edit")
    
    def click(self):
        """
        Click sur l'item.
        """
        self._action("click")

    def dclick(self):
        """
        Double click sur l'item.
        """
        self._action("doubleclick")

class ModelItems(TreeItems):
    """
    Représente des modelitems présents dans un QAbstractModelItem ou dérivé.
    """
    
    ITEM_CLASS = ModelItem

    def item_by_named_path(self, named_path, match_column=0, sep='/', column=0):
        """
        Renvoie l'item (:class:`ModelItem`) correspondant au chemin arborescent
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
        Renvoie la liste de :class:`ModelItem` correspondant à une ligne
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
        while item and parts:
            next_item = None
            part = parts.pop(0)
            for item_ in item.items:
                if match_column == item_.column and item_.value == part:
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

class AbstractItemView(Widget):
    """
    Représente une classe dérivée de QAbstractItemView.
    """
    CPP_CLASS = 'QAbstractItemView'

    def model_items(self):
        """
        Renvoie une instance de :class:`ModelItems` basée sur le modèle
        de cette vue.
        """
        data = self.client.send_command('model_items', oid=self.oid)
        return ModelItems.create(self.client, data)

class TabBar(Widget):
    """
    Représente une classe de QTabBar.
    """
    CPP_CLASS = "QTabBar"

    def tab_texts(self):
        """
        renvoie la liste des textes dans le tabbar.
        """
        data = self.client.send_command('tabbar_list', oid=self.oid)
        return data["tabtexts"]
    
    def set_current_tab(self, tab_index_or_name):
        """
        Définit l'index courant en fonction du texte ou de l'index.
        """
        tabnames = self.tab_texts()
        if isinstance(tab_index_or_name, int):
            index = tab_index_or_name
            if index < 0 or index >= len(tabnames):
                raise ValueError("Index de tab %d invalide" % index)
        else:
            index = tabnames.index(tab_index_or_name)
        self.set_property('currentIndex', index)

class GItem(TreeItem):
    """
    Représente un QGraphicsItem.
    """
    viewid = None
    stackpath = None
    objectname = None
    classes = None
    
    def is_qobject(self):
        return self.objectname != None
    
    def properties(self):
        return self.client.send_command('gitem_properties',
                                         oid=self.viewid,
                                         stackpath=self.stackpath)

class GItems(TreeItems):
    """
    Représente un ensemble de QGraphicsItems
    """
    ITEM_CLASS = GItem

class GraphicsView(Widget):
    CPP_CLASS = 'QGraphicsView'
    
    def gitems(self):
        data = self.client.send_command('graphicsitems', oid=self.oid)
        return GItems.create(self.client, data)
    
    def dump_gitems(self, stream='gitems.json'):
        """
        Ecrit dans un fichier la liste des graphics items.
        """
        data = self.client.send_command('graphicsitems', oid=self.oid)
        if isinstance(stream, basestring):
            stream = open(stream, 'w')
        json.dump(data,
                  stream, sort_keys=True, indent=4, separators=(',', ': '))
