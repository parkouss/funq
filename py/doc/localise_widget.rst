Localiser facilement des widgets
================================

Il existe actuellement deux techniques pour localiser des widgets, qui
sont présentées dans cette section.

Avec **scleHooqAttach**
-----------------------

La technique la plus simple est de lancer **scleHooqAttach** en *mode pick*.

Par exemple, pour localiser les widgets de l'application qtcreator:

.. code-block:: bash
  
  scleHooqAttach --pick kwrite
  
Ensuite, il suffit de cliquer sur un widget en maintenant les touches
*Ctrl* et *Shift* appuyées. Cela affiche sur la sortie standard le chemin
complet du widget et certaines de ses propriétés.

Voici un exemple de sortie lorsque je clique sur le menu "Fichier" de qtcreator::
  
  WIDGET: `Core:_:Internal:_:MainWindow-0::QtCreator.MenuBar` (pos: 42, 12)
  	objectName: QtCreator.MenuBar
  	modal: false
  	windowModality: 0
  	enabled: true
  	x: 0
  	y: 0
  	width: 1091
  	height: 25
  	minimumWidth: 0
  	minimumHeight: 0
  	maximumWidth: 16777215
  	maximumHeight: 16777215
  	font: Sans,10,-1,0,50,0,0,0,0,0
  	mouseTracking: true
  	isActiveWindow: true
  	focusPolicy: 0
  	focus: false
  	contextMenuPolicy: 1
  	updatesEnabled: true
  	visible: true
  	minimized: false
  	maximized: false
  	fullScreen: false
  	acceptDrops: false
  	windowOpacity: 1
  	windowModified: false
  	layoutDirection: 0
  	autoFillBackground: false
  	inputMethodHints: 0
  	defaultUp: false
  	nativeMenuBar: false

Dump xml des widget
-------------------

Il est aussi possible de faire un *dump complet* de l'arborescence des widgets
à un moment donné. Cette fonctionnalité ne peut être utilisée que dans un test.

Voici un exemple d'utilisation::
  from scletest.noseplugin import config
  cfg = config('applitest')
  
  @cfg.with_hooq
  def test_mon_premier_test(hooq):
      # ecriture du dump dans le fichier "dump.xml"
      hooq.dump_widgets_tree('dump.xml')
