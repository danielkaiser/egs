"""
This module provides a unified way of importing EGS plugins.

It will try importing all plugins it finds in the EGS_PATH and make these
available for import via:
import egs.plugins.<plugin_name>
or:
from egs.plugins.<plugin_name> import <object_name>
"""

import importlib
import os
import sys
import warnings


def _load_all_plugins():
    base_plugin_path = os.path.abspath(os.path.join(os.environ['EGS_PATH'], 'plugins'))
    for plugin_name in os.listdir(base_plugin_path):
        if plugin_name.startswith('.'):
            continue
        plugin_path = os.path.join(base_plugin_path, plugin_name)
        if not os.path.isdir(plugin_path):
            continue
        if not os.path.isfile(os.path.join(plugin_path, plugin_name + '.py')):
            continue
        sys.path.append(plugin_path)
        try:
            module = importlib.import_module(plugin_name)
            sys.modules['egs.plugins.' + plugin_name] = module
        except ImportError:
            warnings.warn("Plugin {} could not be loaded!".format(plugin_name))
        sys.path.remove(plugin_path)

_load_all_plugins()
