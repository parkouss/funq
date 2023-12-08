# -*- coding: utf-8 -*-

# Your copyright and license information here

import pytest
from funq.client import ApplicationRegistry
from funq.testcase import register_funq_app_registry
from funq.screenshoter import ScreenShoter
from funq import tools
from configparser import ConfigParser
import os
import codecs
import logging

LOG = logging.getLogger('pytest.plugins.funq')


def pytest_addoption(parser):
    parser.addoption('--funq-conf', default='funq.conf',
                     help="funq configuration file, defaults to `funq.conf`.")
    parser.addoption('--funq-gkit', default='default',
                     help="Choose a specific graphic toolkit.")
    parser.addoption('--funq-gkit-file', default='aliases-gkits.conf',
                     help="Override the file that defines graphic toolkits.")
    parser.addoption('--funq-attach-exe', default=None,
                     help="Complete path to the funq executable.")
    parser.addoption('--funq-trace-tests', default=None,
                     help="A file in which start and end of tests will be logged.")
    parser.addoption('--funq-trace-tests-encoding', default='utf-8',
                     help="encoding of the file used in --funq-trace-tests.")
    parser.addoption('--funq-screenshot-folder', default="screenshot-errors",
                     help="Folder to saves screenshots on error.")
    parser.addoption('--funq-snooze-factor', default=1.0,
                     help="Allow to specify a factor on every timeout.")


@pytest.hookimpl(tryfirst=True)
def pytest_configure(config):
    conf_file = config.getoption('funq_conf')
    if not os.path.isfile(conf_file):
        raise Exception(f"Missing configuration file of funq: `{conf_file}`")

    conf = ConfigParser()
    conf.read([conf_file])

    app_registry = ApplicationRegistry()
    app_registry.register_from_conf(conf, config.option)
    register_funq_app_registry(app_registry)

    trace_tests = config.getoption('funq_trace_tests')
    trace_tests_encoding = config.getoption('funq_trace_tests_encoding')
    screenshoter = ScreenShoter(config.getoption('funq_screenshot_folder'))
    tools.SNOOZE_FACTOR = float(config.getoption('funq_snooze_factor'))

    config.funq_plugin_data = {
        'app_registry': app_registry,
        'trace_tests': trace_tests,
        'trace_tests_encoding': trace_tests_encoding,
        'screenshoter': screenshoter,
    }


def pytest_runtest_protocol(item, nextitem):
    message = f"Starting test `{item.name}`"
    lines = '-' * 70
    LOG.info(f"{lines}\n{message}\n{lines}")

    trace_tests = item.config.funq_plugin_data['trace_tests']
    trace_tests_encoding = item.config.funq_plugin_data['trace_tests_encoding']

    if trace_tests:
        with codecs.open(trace_tests, 'a', trace_tests_encoding) as f:
            f.write(f"{lines}\n{message}\n{lines}\n")

    yield

    message = f"Ending test `{item.name}`"
    LOG.info(f"{lines}\n{message}\n{lines}")

    if trace_tests:
        with codecs.open(trace_tests, 'a', trace_tests_encoding) as f:
            f.write(f"{lines}\n{message}\n{lines}\n")


@pytest.hookimpl(tryfirst=True)
def pytest_runtest_makereport(item, call):
    if call.excinfo is not None:
        screenshoter = item.config.funq_plugin_data['screenshoter']
        # Replace this with your logic to take a screenshot
        screenshoter.take_screenshot(None, item.name)
