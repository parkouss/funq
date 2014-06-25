# -*- coding: utf-8 -*-
"""
Ce module permet l'intégration avec un serveur libFunq via :class:`FunqClient`.
"""

import socket, json, errno, os, shlex, subprocess, base64
from functools import wraps
from collections import defaultdict
import logging

from funq.aliases import HooqAliases
from funq.tools import wait_for
from funq.models import Widget
from funq.errors import FunqError, TimeOutError
from unittest.case import SkipTest
from funq import screenshoter

LOG = logging.getLogger('funq.client')

class FunqClient(object):
    """
    Permet l'échange de messages avec un serveur libFunq.
    """
    DEFAULT_HOST = 'localhost'
    DEFAULT_PORT = 9999
    
    def __init__(self, host=None, port=None, aliases=None,
                       timeout_connection=10):
        if host is None:
            host = self.DEFAULT_HOST
        if port is None:
            port = self.DEFAULT_PORT
        
        if aliases is None:
            aliases = HooqAliases()
        elif isinstance(aliases, basestring):
            aliases = HooqAliases.from_file(aliases)
        elif not isinstance(aliases, HooqAliases):
            raise TypeError("aliases must be None or str or an"
                             " instance of HooqAliases")
        
        self.aliases = aliases
        
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        def connect():
            """ tentative de connexion """
            try:
                self._socket.connect((host, port))
                return True
            except socket.error, e:
                if e.errno != errno.ECONNREFUSED:
                    raise
                return e
        
        wait_for(connect, timeout_connection, 0.2)
        self._fsocket = self._socket.makefile() 

    def close(self):
        """
        Ferme le socket de connexion avec libFunq.
        
        L'objet devient inutilisable après appel de cette méthode.
        Cette méthode est appelée automatiquement sur destruction de l'objet.
        """
        self._socket.close()
    
    def __del__(self):
        self.close()

    def _raw_send(self, action, kwargs):
        """
        Envoi de message, sans attente de réception.
        """
        kwargs['action'] = action
        rawdata = json.dumps(kwargs)
        message = '%s\n%s' % (len(rawdata), rawdata)
        f = self._fsocket
        f.write(message)
        f.flush()

    def send_command(self, action, **kwargs):
        """
        Envoi un message au serveur libFunq, et retourne la réponse décodée.
        
        :raises: :class:`funq.errors.FunqError`
        """
        self._raw_send(action, kwargs)
        f = self._fsocket
        header = f.readline()
        if not header:
            raise FunqError("NoResponseFromApplication",
                            u"Pas de réponse de l'application testée -"
                            u" probablement un crash.")
        to_read = int(header)
        response = json.loads(f.read(to_read))
        if response.get('success') == False:
            raise FunqError(response["errName"], response["errDesc"])
        return response

    def quit(self):
        """
        Provoque un appel de qApp->quit() dans l'application testée.
        """
        self._raw_send('quit', {})
    
    def widget(self, alias=None, path=None, # pylint:disable=R0913
                     timeout=10.0, timeout_interval=0.1, wait_active=True):
        """
        Retourne un widget de type :class:`funq.models.Widget` ou dérivé
        identifié par un alias ou le path complet.
        
        Exemple::
          
          widget = client.widget('mon_alias')
        
        :param alias: alias défini dans le fichier d'alias
        :param path: path complet identifiant le widget
        :param timeout: si > 0, retente de récupérer le widget si échec jusqu'à
                        la valeur de timeout
        :param timeout_interval: temps d'attente entre chque demande de
                                 récupération de widget
        :param wait_active: Si True, on attends que le widget soit visible
                            et enabled.
        """
        if not (alias or path):
            raise TypeError("alias or path must be defined")
        
        if alias:
            path = self.aliases[alias]
        
        wdata = [None]
        def get_widget():
            """ Tente de récupérer un widget """
            try:
                wdata[0] = self.send_command('widget_by_path', path=path)
                return True
            except FunqError, err:
                if err.classname != 'InvalidWidgetPath':
                    raise
                return err
        wait_for(get_widget, timeout, timeout_interval)
        
        widget = Widget.create(self, wdata[0])
        if wait_active:
            widget.wait_for_properties({'enabled': True, 'visible': True})
        return widget
    
    def active_window(self, timeout=10.0, timeout_interval=0.1, wait_active=True):
        """
        Retourne un widget de type :class:`funq.models.Widget` ou dérivé
        représentant le widget actif de l'application.
        
        Exemple::
          
          active_widget = client.active_window()
        
        :param timeout: si > 0, retente de récupérer le widget si échec jusqu'à
                        la valeur de timeout
        :param timeout_interval: temps d'attente entre chque demande de
                                 récupération de widget
        :param wait_active: Si True, on attends que le widget soit visible
                            et enabled.
        """
        wdata = [None]
        def get_widget():
            """ Tente de récupérer le widget actif """
            try:
                wdata[0] = self.send_command('active_window')
                return True
            except FunqError, err:
                if err.classname != 'NoActiveWindow':
                    raise
                return err
        wait_for(get_widget, timeout, timeout_interval)
        
        widget = Widget.create(self, wdata[0])
        if wait_active:
            widget.wait_for_properties({'enabled': True, 'visible': True})
        return widget
    
    def widgets_list(self, with_properties=False):
        """
        Renvoie un dictionnaire de la liste des widgets actuels de l'application
        testée.
        """
        return self.send_command('widgets_list',
                                  with_properties=with_properties)
    
    def dump_widgets_list(self, stream='widgets_list.json',
                                 with_properties=False):
        """
        Ecrit dans un fichier le résultat de :meth:`widgets_list`.
        """
        if isinstance(stream, basestring):
            stream = open(stream, 'w')
        json.dump(self.widgets_list(with_properties=with_properties),
                  stream, sort_keys=True, indent=4, separators=(',', ': '))

    def take_screenshot(self, stream='screenshot.png', format_='PNG'):
        """
        Prends un screenshot du desktop actif.
        """
        data = self.send_command('desktop_screenshot', format=format_)
        if isinstance(stream, basestring):
            stream = open(stream, 'wb')
        raw = base64.standard_b64decode(data['data'])
        stream.write(raw) #pylint: disable=E1103
    
    def keyclick(self, text):
        """
        Simule les évènements keypress et keyrelease pour chaque lettre du texte
        passé.
        """
        self.send_command('widget_keyclick', text=text)
    
    def shortcut(self, key_sequence):
        """
        Envoi un raccourci clavier, féfini par une séquence de texte. Le
        format de la séquence est défini par QKeySequence::fromString.
        
        Exemple::
          
          client.shortcut('F2')
        
        """
        self.send_command('shortcut', keysequence=key_sequence)
    
    def drag_n_drop(self, src_widget, src_pos=None,
                          dest_widget=None, dest_pos=None):
        """
        Effectue un drag and drop.
        
        :param src_widget: widget source
        :param src_pos: position de début de drag. Si None, le centre de
                        src_widget sera utilisé. Doit être un tuple (x,y).
        :param dest_widget: widget de destination. Si none, src_widget sera
                            utilisé.
        :param dest_pos: position de fin de drag. Si None, le centre de
                         dest_widget sera utilisé. Doit être un tuple (x,y).
        """
        if dest_widget is None:
            dest_widget = src_widget
        if src_pos is not None:
            src_pos = ','.join(map(str, src_pos))
        if dest_pos is not None:
            dest_pos = ','.join(map(str, dest_pos))
        self.send_command("drag_n_drop",
                          srcoid=src_widget.oid,
                          destoid=dest_widget.oid,
                          srcpos=src_pos,
                          destpos=dest_pos)

class ApplicationContext(object): # pylint: disable=R0903
    """
    Représente le contexte d'une application testée.
    
    L'instanciation de cette classe peut lancer l'exécutable à tester
    avec funq (si appconfig.executable ne commence pas par 
    "socket://").
    
    Ensuite on tente de se connecter au serveur via une instance de
    :class:`FunqClient` accessible par la variable membre **funq**.
    
    A la destruction de cette instance, la méthode :meth:`terminate`
    est automatiquement appellée et ferme l'objet **funq** ainsi que le
    processus testé s'il a été créé.
    """
    def __init__(self, appconfig, client_class=FunqClient):
        self._process, self.funq = None, None
        
        if not appconfig.executable.startswith('socket://'):
            self._start_test_process(appconfig)
            host = None # means localhost
        else:
            host = appconfig.executable[9:]
        
        self.funq = client_class(host=host,
                               port=appconfig.funq_port,
                               aliases=appconfig.create_aliases(),
                               timeout_connection=appconfig.timeout_connection)
    
    def _start_test_process(self, appconfig):
        """
        Demarre le process de l'appli à tester.
        """
        
        env = appconfig.env
        cmd = []
        funq_port = appconfig.funq_port
        
        stdout = appconfig.executable_stdout
        stderr = appconfig.executable_stderr
        
        if stderr:
            if stderr == stdout:
                stderr = subprocess.STDOUT
            else:
                stderr = open(stderr, 'a')
        if stdout:
            stdout = open(stdout, 'a')
        
        if not appconfig.attach:
            # le process integre libFunq dans le code compilé.
            if env is None:
                env = os.environ
            
            # copy env
            env = dict(env.items())
            
            env['FUNQ_ACTIVATION'] = '1'
            if funq_port:
                env['FUNQ_PORT'] = str(funq_port)
            
        else:
            # injection de dll par l'utilisation de funq
            if not appconfig.global_options.funq_attach_exe:
                raise RuntimeError("Pour utiliser funq, il faut"
                                    " specifier l'option --funq-attach-exe de"
                                    " nose ou positionner funq dans"
                                    " le PATH.")
            cmd = [appconfig.global_options.funq_attach_exe]
            if funq_port:
                cmd.append('--port')
                cmd.append(str(funq_port))
        
        if appconfig.with_valgrind:
            cmd.append('valgrind')
            cmd.extend(appconfig.valgrind_args)
        cmd.append(appconfig.executable)
        
        cmd.extend(appconfig.args)
        
        LOG.info("L'application testée va être démarrée dans le répertoire %r"
                 " avec la commande %r", appconfig.cwd, cmd)
        self._process = subprocess.Popen(cmd,
                                         cwd=appconfig.cwd,
                                         stdout=stdout,
                                         stderr=stderr,
                                         env=env)
        LOG.info("Démarrage de l'application testée [%s].", self._process.pid)
    
    def _kill_process(self):
        """
        Tue le process de l'application de test
        """
        if self._process:
            # attente de fermeture gentille
            try:
                wait_for(lambda: self._process.poll() is not None, 10, 0.05)
            except TimeOutError:
                pass
            if self._process.returncode is None:
                # application bloquée ! pas le choix ...
                LOG.warn("L'application testée [%s] ne veut pas se terminer"
                         " gentiment, on force son arrêt.", self._process.pid)
                self._process.terminate()
                self._process.wait()
            self._process = None
    
    def terminate(self):
        """
        Tente de tuer le process de test et ferme l'objet **hooq**.
        """
        if self.funq:
            if self._process is not None:
                # le process peut être mort, et ne pas nous l'avoir signalé
                try:
                    wait_for(lambda: self._process.poll() is not None,
                             0.05,
                             0.01)
                except TimeOutError:
                    pass
                if self._process.returncode is not None:
                    # process fini de manière innatendue (-11: SegFault)
                    LOG.critical("L'application testée [%s] s'est terminée de"
                                 " manière innatendue (code retour: %s)",
                                 self._process.pid, self._process.returncode)
                    self._process = None
                else:
                    # mode attache, on doit gerer la fin du process
                    # demande de fermeture, gentiment (qApp->quit()).
                    LOG.info("Fermeture de l'application testée [%s].",
                             self._process.pid)
                    try:
                        self.funq.quit()
                    except socket.error:
                        pass
            try:
                self.funq.close()
            except socket.error:
                pass
            self.funq = None
        self._kill_process()
    
    def __del__(self):
        self.terminate()

class ApplicationConfig(object): # pylint: disable=R0902
    """
    Cet objet permet de créer des :class:`ApplicationContext`. Chaque
    paramètre est accessible sur l'instance, ce qui permet par exemple de
    récupérer le chemin de l'exécutable testé via *config.executable*.
    
    :param executable: chemin complet vers l'exécutable de test
    :param args: arguments de l'exécutable
    :param funq_port: Spécifie un port pour la communication libFunq
    :param cwd: Répertoire d'exécution de l'exécutable. Si None,
                l'exécutable sera lancé depuis son propre dossier
    :param env: dict représentant les variables d'environnement à passer
                lors de l'exécution de l'exécutable. Si None, os.environ
                est utilisé
    :param timeout_connection: temps d'attente maximum avant de déclarer
                              forfait pour la connexion libFunq.
    """
    def __init__(self, executable, # pylint: disable=R0913
                       args=(),
                       funq_port=None,
                       cwd=None,
                       env=None,
                       timeout_connection=10,
                       aliases=None,
                       executable_stdout=None,
                       executable_stderr=None,
                       attach=True,
                       screenshot_on_error=False,
                       with_valgrind=False,
                       valgrind_args=('--leak-check=full',
                                      '--show-reachable=yes'),
                       global_options=None):
        self.executable = executable
        self.args = args
        self.funq_port = funq_port
        self.cwd = cwd or os.path.dirname(executable) or os.getcwd()
        self.env = env
        self.timeout_connection = timeout_connection
        self.aliases = aliases
        self.executable_stdout = executable_stdout
        self.executable_stderr = executable_stderr
        self.attach = attach
        self.screenshot_on_error = screenshot_on_error
        self.with_valgrind = with_valgrind
        self.valgrind_args = valgrind_args
        self.global_options = global_options
    
    def create_aliases(self):
        """
        Crée et renvoie un objet :class:`HooqAliases` selon la config
        """
        if not self.aliases:
            return None
        return HooqAliases.from_file(self.aliases,
                    self.global_options.funq_gkit_file,
                    self.global_options.funq_gkit)
    
    @classmethod
    def from_conf(cls, conf, section, global_options): # pylint:disable=R0912
        """
        Génère une instance de :class:`ApplicationConfig` à partir
        d'un fichier de configuration lu par ConfigParser.
        """
        
        basedir = os.path.dirname(global_options.funq_conf)
        
        executable = conf.get(section, 'executable')
        if not executable.startswith('socket://') and (
                        basedir and not os.path.isabs(executable)):
            executable = os.path.join(basedir, executable)
        
        kwargs = {'global_options': global_options}
        if conf.has_option(section, 'args'):
            kwargs['args'] = shlex.split(conf.get(section, 'args'))

        if conf.has_option(section, 'funq_port'):
            kwargs['funq_port'] = conf.getint(section, 'funq_port')

        if conf.has_option(section, 'timeout_connection'):
            kwargs['timeout_connection'] = conf.getint(section,
                                             'timeout_connection')
        
        if conf.has_option(section, 'attach'):
            kwargs["attach"] = conf.getboolean(section, 'attach')
        
        for optname in ('cwd', 'aliases', 'executable_stdout',
                         'executable_stderr'):
            if conf.has_option(section, optname):
                kwargs[optname] = conf.get(section, optname)
                if basedir and not os.path.isabs(kwargs[optname]):
                    kwargs[optname] = os.path.join(basedir, kwargs[optname])
        
        # devnull si NULL spécifié dans le fichier de conf
        for optname in ('executable_stdout', 'executable_stderr'):
            if conf.has_option(section, optname) and \
                            conf.get(section, optname) == 'NULL':
                kwargs[optname] = os.devnull
        
        if conf.has_option(section, 'with_valgrind'):
            kwargs["with_valgrind"] = conf.getboolean(section, 'with_valgrind')
        
        if conf.has_option(section, 'valgrind_args'):
            kwargs['valgrind_args'] = shlex.split(
                                            conf.get(section, 'valgrind_args'))
        
        if conf.has_option(section, 'screenshot_on_error'):
            kwargs["screenshot_on_error"] = conf.getboolean(section,
                                                        'screenshot_on_error')
        
        return cls(executable, **kwargs) # pylint: disable=W0142

    def create_context(self):
        """
        Retourne une instance de :class:`ApplicationContext`.
        """
        return ApplicationContext(self)
    
    def with_hooq(self, meth):
        """
        Décorateur simple de fonction permettant de créer un contexte
        qui sera passé en argument et automatiquement détruit après
        exécution de cette fonction.
        
        La fonction décorée prendra un argument, généralement nommé *hooq*
        de type :class:`FunqClient`.
        """
        @wraps(meth)
        def wrapper():
            """ Implémentation du décorateur"""
            ctx = self.create_context()
            try:
                return meth(ctx.funq)
            except (SystemExit, KeyboardInterrupt, SkipTest):
                raise
            except:
                if self.screenshot_on_error:
                    screenshoter.take_screenshot(ctx.funq)
                raise
        return wrapper

class MultiApplicationConfig(tuple):
    """
    Permet de manipuler plusieurs applications de test en même temps.
    
    Cette classe agit comme un tuple de :class:`ApplicationConfig`,
    donc chaque ApplicationConfig peut être récupérée par l'utilisation
    de l'opérateur [] basé sur l'index de la configuration voulue.
    """
    def __init__(self, appconfigs):
        tuple.__init__(appconfigs)
    
    def with_hooq(self, meth):
        """
        Décorateur simple de fonction permettant de créer les contextes
        des :class:`ApplicationConfig` stockées qui seront
        automatiquement détruits après exécution de la fonction.
        
        La fonction décorée prendra autant d'arguments que l'instance de
        MultiApplicationConfig contient d'ApplicationConfig,
        de type :class:`FunqClient`.
        """
        @wraps(meth)
        def wrapper():
            """ Implémentation du décorateur"""
            ctxs = [ appconfig.create_context() for appconfig
                     in self]
            return meth(*[ctx.funq for ctx in ctxs])
        return wrapper

class ApplicationRegistry(object):
    """
    Gère un ensemble de :class:`ApplicationConfig`. Une instance
    globale est utilisée dans :mod:`funq.noseplugin` pour stocker
    les ApplicationConfig déclarées par le fichier de conf.
    """
    def __init__(self):
        self.confs = defaultdict(dict)
    
    def register_from_conf(self, conf, global_options):
        """
        Enregistre des configs depuis un fichier de conf.
        """
        for section in conf.sections():
            if ':' in section:
                app, mode = section.split(':', 1)
            else:
                app, mode = section, 'default'
            appconf = ApplicationConfig.from_conf(conf, section, global_options)
            self.register_config(app, appconf, mode)
    
    def register_config(self, name, conf, mode='default'):
        """ Enregistre la config *name* """
        self.confs[name][mode] = conf
    
    def config(self, name, mode='default'):
        """
        Retourne la configuration *name* de type :class:`ApplicationConfig`.
        
        :param name: nom de la configuration
        :param mode: actuellement non utilisé
        """
        return self.confs[name][mode]
    
    def multi_config(self, names, mode='default'):
        """
        Retourne la liste de configurations *names* sous la forme d'une instance
        de :class:`MultiApplicationConfig`.
        
        :param names: liste ordonnée des noms de configuration
        :param mode: actuellement non utilisé
        """
        return MultiApplicationConfig(
                    [ self.config(name, mode) for name in names ])
