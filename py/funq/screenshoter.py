# -*- coding: utf-8 -*-
"""
Permet de prendre des screenshots et de les nommer de manière automatique.

Le module doit être utilisé de la manière suivante::
  
  from funq import screenshoter
  
  screenshoter.init('/path/vers/dossier/images')
  
  # puis utilisation autre part
  from funq import screenshoter
  
  screenshoter.take_screenshot(hooq)
"""

import os, itertools, logging, codecs

LOG = logging.getLogger('funq.screenshoter')

class ScreenShoter(object): # pylint: disable=R0903
    """
    Objet permettant de prendre des screenshot.
    Actuellement, cette classe n'est utilisée
    """
    def __init__(self, working_folder):
        self.working_folder = os.path.realpath(working_folder)
        self.txt_file_path = os.path.join(self.working_folder, 'images.txt')
        self.counter = itertools.count()
        
        if os.path.isfile(self.txt_file_path):
            os.unlink(self.txt_file_path)
    
    def take_screenshot(self, funqclient, longname):
        """
        Prends un screenshot de l'application passée en argument, en y
        associant le nom `longname` donné dans le fichier images.txt.
        """
        if not os.path.isdir(self.working_folder):
            os.makedirs(self.working_folder)
        
        bname = '{0}.png'.format(self.counter.next())
        fname = os.path.join(self.working_folder, bname)
        
        try:
            funqclient.take_screenshot(fname, 'PNG')
        except (SystemExit, KeyboardInterrupt):
            raise
        except:
            LOG.exception(u"impossible de prendre un screenshot pour"
                          u" %s", longname)
            return
        
        with codecs.open(self.txt_file_path, "a", "utf-8") as f:
            f.write(u"{0}: {1}\n".format(bname, longname))

_INSTANCE = None

def init(*args, **kwargs):
    """
    Initialise screenshoter
    """
    global _INSTANCE # pylint: disable=W0603
    _INSTANCE = ScreenShoter(*args, **kwargs)

def take_screenshot(funqclient):
    """
    Permet actuellement de prendre un screenshot, en utilisant le nom du
    test en cours pour identifier le screenshot.
    """
    global _INSTANCE # pylint: disable=W0603
    if _INSTANCE is None:
        return
    from funq.noseplugin import FunqPlugin
    _INSTANCE.take_screenshot(funqclient, FunqPlugin.current_test_name())
