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
This module allow to communicate with a libFunq server with
:class:`FunqClient`.
"""

import socket
import json
import errno
import os
import shlex
import subprocess
import base64
from collections import defaultdict
import logging

from funq.aliases import HooqAliases
from funq.tools import wait_for
from funq.models import Action, Widget
from funq.errors import FunqError, TimeOutError

LOG = logging.getLogger('funq.client')


# python 3 compatibility
# https://stackoverflow.com/questions/11301138/how-to-check-if-variable-is-string-with-python-2-and-3-compatibility)
try:
    basestring
except NameError:
    basestring = str


class FunqClient(object):

    """
    Allow to communicate with a libFunq server.

    This is the main class used to manipulate tested application.
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

        def connect():
            """ try to connect """
            try:
                self._socket = socket.socket(socket.AF_INET,
                                             socket.SOCK_STREAM)
                self._socket.connect((host, port))
                return True
            except socket.error as e:
                if e.errno != errno.ECONNREFUSED:
                    raise
                return e

        wait_for(connect, timeout_connection, 0.2)
        self._socket.settimeout(timeout_connection)
        self._fsocket = self._socket.makefile(mode="rwb")

    def duplicate(self):
        """
        Allow to manipulate the application in another thread.

        Returns a new instance of :class:`FunqClient` with a new socket.

        Example::

          # `client_copy` may be used in concurrence with `client`.
          client_copy = client.duplicate()
        """
        host, port = self._socket.getpeername()
        return FunqClient(host=host, port=port, aliases=self.aliases)

    def close(self):
        """
        Close the libFunq socket.

        The instance become useless after this method is called. This
        method is automatically called on the object destruction.
        """
        self._socket.close()

    def __del__(self):
        self.close()

    def _raw_send(self, action, kwargs):
        """
        Send a message without waiting for an answer.
        """
        kwargs['action'] = action
        rawdata = json.dumps(kwargs).encode('utf-8')
        header = '{}\n'.format(len(rawdata)).encode('utf-8')
        message = header + rawdata
        f = self._fsocket
        f.write(message)
        f.flush()

    def send_command(self, action, **kwargs):
        """
        Send a message to the libFunq server and returns the decoded
        answer.

        :raises: :class:`funq.errors.FunqError` on error
        """
        self._raw_send(action, kwargs)
        f = self._fsocket
        header = f.readline()
        if not header:
            raise FunqError("NoResponseFromApplication",
                            u"Pas de réponse de l'application testée -"
                            u" probablement un crash.")
        to_read = int(header)
        response = json.loads(f.read(to_read).decode('utf-8'))
        if response.get('success') is False:
            raise FunqError(response["errName"], response["errDesc"])
        return response

    def quit(self):
        """
        Ask the tested application to quit by calling qApp->quit().
        """
        self._raw_send('quit', {})

    def action(self, alias=None, path=None, timeout=10.0,
               timeout_interval=0.1, wait_active=True):
        """
        Returns an instance of a :class:`funq.models.Action` or derived
        identified with an alias or with its complete path.

        Example::

          action = client.action('my_alias')

        :param alias: alias defined in the aliases file.
        :param path: complete path for the action
        :param timeout: if > 0, tries to get the action until timeout
                        is reached (second)
        :param timeout_interval: time between two atempts to get an action
                                 (seconds)
        :param wait_active: If true - the default -, wait until the action
                            become visible and enabled.
        """
        if not (alias or path):
            raise TypeError("alias or path must be defined")

        if alias:
            path = self.aliases[alias]

        wdata = [None]

        def get_action():
            """ Try to get the action """
            try:
                wdata[0] = self.send_command('widget_by_path', path=path)
                return True
            except FunqError as err:
                if err.classname != 'InvalidWidgetPath':
                    raise
                return err
        wait_for(get_action, timeout, timeout_interval)

        action = Action.create(self, wdata[0])
        if wait_active:
            action.wait_for_properties({'enabled': True, 'visible': True})
        return action

    def widget(self, alias=None, path=None, timeout=10.0,
               timeout_interval=0.1, wait_active=True):
        """
        Returns an instance of a :class:`funq.models.Widget` or derived
        identified with an alias or with its complete path.

        Example::

          widget = client.widget('my_alias')

        :param alias: alias defined in the aliases file.
        :param path: complete path for the widget
        :param timeout: if > 0, tries to get the widget until timeout
                        is reached (second)
        :param timeout_interval: time between two atempts to get a widget
                                 (seconds)
        :param wait_active: If true - the default -, wait until the widget
                            become visible and enabled.
        """
        if not (alias or path):
            raise TypeError("alias or path must be defined")

        if alias:
            path = self.aliases[alias]

        wdata = [None]

        def get_widget():
            """ Try to get the widget """
            try:
                wdata[0] = self.send_command('widget_by_path', path=path)
                return True
            except FunqError as err:
                if err.classname != 'InvalidWidgetPath':
                    raise
                return err
        wait_for(get_widget, timeout, timeout_interval)

        widget = Widget.create(self, wdata[0])
        if wait_active:
            if 'QWindow' in wdata[0]['classes']:
                # QWindow (Qt5) does not have the enabled property
                props = {'active': True, 'visible': True}
            else:
                props = {'enabled': True, 'visible': True}
            widget.wait_for_properties(props)
        return widget

    def active_widget(self, widget_type='window', timeout=10.0,
                      timeout_interval=0.1, wait_active=True):
        """
        Returns an instance of a :class:`funq.models.Widget` or derived
        that is the active widget of the application, or the widget that
        got the focus.

        Be careful, this method acts weidly under Xvfb.

        Example::

          my_dialog = client.active_window('modal')

        :param widget_type: kind of widget. ('window', 'modal', 'popup'
                            ou 'focus'
                            -> see the QT documentation about
                            QApplication::activeWindow,
                            QApplication::activeModalWidget,
                            QApplication::activePopupWidget or
                            QApplication::focusWidget respectively)
        :param timeout: if > 0, tries to get the widget until timeout
                        is reached (second)
        :param timeout_interval: time between two atempts to get a widget
                                 (seconds)
        :param wait_active: If true - the default -, wait until the widget
                            become visible and enabled.
        """
        wdata = [None]

        def get_widget():
            """ Try to get the widget """
            try:
                wdata[0] = self.send_command('active_widget', type=widget_type)
                return True
            except FunqError as err:
                if err.classname != 'NoActiveWindow':
                    raise
                return err
        wait_for(get_widget, timeout, timeout_interval)

        widget = Widget.create(self, wdata[0])
        if wait_active:
            if 'QWindow' in wdata[0]['classes']:
                # QWindow (Qt5) does not have the enabled property
                props = {'active': True, 'visible': True}
            else:
                props = {'enabled': True, 'visible': True}
            widget.wait_for_properties(props)
        return widget

    def widgets_list(self, with_properties=False):
        """
        Returns a dict with every widgets in the application.
        """
        return self.send_command('widgets_list',
                                 with_properties=with_properties)

    def dump_widgets_list(self, stream='widgets_list.json',
                          with_properties=False):
        """
        Write in a file the result of :meth:`widgets_list`.
        """
        if isinstance(stream, basestring):
            stream = open(stream, 'w')
        json.dump(self.widgets_list(with_properties=with_properties),
                  stream, sort_keys=True, indent=4, separators=(',', ': '))

    def take_screenshot(self, stream='screenshot.png', format_='PNG'):
        """
        Take a screenshot of the active desktop.
        """
        data = self.send_command('grab', format=format_)
        if isinstance(stream, basestring):
            stream = open(stream, 'wb')
        raw = base64.standard_b64decode(data['data'])
        stream.write(raw)  # pylint: disable=E1103

    def keyclick(self, text):
        """
        Simulate keyboard entry by sending keypress and keyrelease events
        for each character of the given text.
        """
        self.send_command('widget_keyclick', text=text)

    def shortcut(self, key_sequence):
        """
        Send a shortcut defined with a text sequence. The format of this
        text sequence is defined with QKeySequence::fromString (see QT
        documentation for more details).

        Example::

          client.shortcut('F2')

        """
        self.send_command('shortcut', keysequence=key_sequence)

    def drag_n_drop(self, src_widget, src_pos=None,
                    dest_widget=None, dest_pos=None):
        """
        Do a drag and drop.

        :param src_widget: source widget
        :param src_pos: starting position for the drag. If None, the center
                        of `src_widget` will be used, else it must be a
                        tuple (x, y) in widget coordinates.
        :param dest_widget: destination widget. If None, src_widget will
                            be used.
        :param dest_pos: ending position for the drop. If None, the center
                         of `dest_widget` will be used, else it must be a
                         tuple (x, y) in widget coordinates.
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


class ApplicationContext(object):  # pylint: disable=R0903

    """
    This is the context of a tested application.

    Instanciate this class may launch the tested application with funq
    (if appconfig.executable does not starts with "socket://").

    Then it will try to connect to the libFunq server with a
    :class:`FunqClient` accessible from the member **funq**.

    When the instance is garbage collected, :meth:`terminate` is
    automatically called to close the **funq** member and terminate
    the tested application process.
    """

    def __init__(self, appconfig, client_class=FunqClient):
        self._process, self.funq = None, None

        if not appconfig.executable.startswith('socket://'):
            self._start_test_process(appconfig)
            host = None  # means localhost
        else:
            host = appconfig.executable[9:]

        self.funq = client_class(
            host=host,
            port=appconfig.funq_port,
            aliases=appconfig.create_aliases(),
            timeout_connection=appconfig.timeout_connection
        )

    def _start_test_process(self, appconfig):
        """
        Start the process of the tested application.
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
            # libFunq is compiled inside the tested application binary
            if env is None:
                env = os.environ

            # copy env
            env = dict(env.items())

            env['FUNQ_ACTIVATION'] = '1'
            if funq_port:
                env['FUNQ_PORT'] = str(funq_port)

        else:
            # inject libFunq with funq executable
            if not appconfig.global_options.funq_attach_exe:
                raise RuntimeError("To use funq, you have to specify the"
                                   " nose option --funq-attach-exe"
                                   " or put the funq executable in PATH")
            cmd = [appconfig.global_options.funq_attach_exe]
            if funq_port:
                cmd.append('--port')
                cmd.append(str(funq_port))

        if appconfig.with_valgrind:
            cmd.append('valgrind')
            cmd.extend(appconfig.valgrind_args)
        cmd.append(appconfig.executable)

        cmd.extend(appconfig.args)

        LOG.info("The tested application will be launched in the"
                 " directory %r with the command %r", appconfig.cwd, cmd)
        self._process = subprocess.Popen(cmd,
                                         cwd=appconfig.cwd,
                                         stdout=stdout,
                                         stderr=stderr,
                                         env=env)
        LOG.info("Launching tested application [%s].", self._process.pid)

    def _kill_process(self):
        """
        Kill the application tested process
        """
        if self._process:
            # wait for a nice exit
            try:
                wait_for(lambda: self._process.poll() is not None, 10, 0.05)
            except TimeOutError:
                pass
            if self._process.returncode is None:
                # application seems blocked ! try to terminate it ...
                LOG.warn("The tested application [%s] can not be stopped"
                         " nicely.", self._process.pid)
                self._process.terminate()
                self._process.wait()
            self._process = None

    def terminate(self):
        """
        Try to kill the process and close the **funq** object.
        """
        if self.funq:
            if self._process is not None:
                # the process may be already dead
                try:
                    wait_for(lambda: self._process.poll() is not None,
                             0.05,
                             0.01)
                except TimeOutError:
                    pass
                if self._process.returncode is not None:
                    # process terminated unexpectedly (-11: SegFault)
                    LOG.critical("The tested application [%s] has terminated"
                                 " unexpectedly (return code: %s)",
                                 self._process.pid, self._process.returncode)
                    self._process = None
                else:
                    # try to exit nicely the tested application process
                    # with a call to qApp->quit().
                    LOG.info("Closing tested application [%s].",
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


class ApplicationConfig(object):  # pylint: disable=R0902

    """
    This object hold the configuration of the application to test, mostly
    retrieved from the funq configuration file.

    Each parameter is accessible on the instance, allowing to retrieve
    the tested application path for example with *config.executable*,
    or its exeution path with *config.cwd*.

    :param executable: complete path to the tested application
    :param args: executable arguments
    :param funq_port: socket port number for the libFunq connection
    :param cwd: execution path for the tested application. If None, the
                value will be the directory of executable.
    :param env: dict environment variables. If None, os.environ will be
                used.
    :param timeout_connection: timeout to try to connect to libFunq.
    :param aliases: path to the aliases file
    :param executable_stdout: file path to redirect stdout or None.
    :param executable_stderr: file path to redirect stderr or None.
    :param attach: Indicate if the process is attached or if it is a
                   distant connection.
    :param screenshot_on_error: Indicate if screenshots must be taken
                                on errors.
    :param with_valgrind: indicate if valgrind must be used.
    :param valgrind_args: valgrind arguments
    :param global_options: options from the funq nose plugin.
    """

    def __init__(self, executable,  # pylint: disable=R0913
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
        Create and returns and aliases object.
        """
        if not self.aliases:
            return None
        return HooqAliases.from_file(self.aliases,
                                     self.global_options.funq_gkit_file,
                                     self.global_options.funq_gkit)

    @classmethod
    def from_conf(cls, conf, section, global_options):
        """
        Create an instance of :class:`ApplicationConfig` from a
        funq configuration section.
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
            if kwargs['funq_port'] == 0 and \
                    not executable.startswith('socket://'):
                # take an available port
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.bind(('', 0))
                kwargs['funq_port'] = sock.getsockname()[1]
                sock.close()
                del sock

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

        # devnull if NULL specified in the config file
        for optname in ('executable_stdout', 'executable_stderr'):
            if conf.has_option(section, optname) and \
                    conf.get(section, optname) == 'NULL':
                kwargs[optname] = os.devnull

        if conf.has_option(section, 'with_valgrind'):
            kwargs["with_valgrind"] = \
                conf.getboolean(section, 'with_valgrind')

        if conf.has_option(section, 'valgrind_args'):
            kwargs['valgrind_args'] = \
                shlex.split(conf.get(section, 'valgrind_args'))

        if conf.has_option(section, 'screenshot_on_error'):
            kwargs["screenshot_on_error"] = \
                conf.getboolean(section, 'screenshot_on_error')

        return cls(executable, **kwargs)


class ApplicationRegistry(object):

    """
    Handle multiple :class:`ApplicationConfig`. A global instance is
    used in :mod:`funq.noseplugin` to keep every configuration defined
    in the funq configuration file.
    """

    def __init__(self):
        self.confs = defaultdict(dict)

    def register_from_conf(self, conf, global_options):
        """
        Save configurations given a funq config.
        """
        for section in conf.sections():
            if ':' in section:
                app, mode = section.split(':', 1)
            else:
                app = section
            appconf = ApplicationConfig.from_conf(
                conf, section, global_options)
            self.register_config(app, appconf)

    def register_config(self, name, conf):
        """ Save the config *name* """
        self.confs[name] = conf

    def config(self, name):
        """
        Returns the :class:`ApplicationConfig` associated to *name*.

        :param name: name of the configuration
        """
        return self.confs[name]
