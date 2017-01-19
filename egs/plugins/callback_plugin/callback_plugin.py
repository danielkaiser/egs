# coding: utf-8
"""
Callback Plugin for executing generic functions at draw time

This plugin allows the user to register a callback function (or any other type
of callable), so that it will be executed whenever the corresponding display
list is executed. This can be used by applications for the ad-hoc creation of custom
display list items, without the need to create a complete plugin.

>>> import callback_plugin
>>> callback = Callback(lambda ctx: print("Drawing..."))

"""

import ctypes
import sys
import os
from sys import platform as _platform

from egs import DisplayListElem, Context, GLContext, LIB_SUFFIX

LIB_NAME = 'libcallback_plugin.'+LIB_SUFFIX

_callback_plugin = ctypes.cdll.LoadLibrary(os.path.join(os.environ.get('EGS_PATH'), 'plugins', 'callback_plugin', LIB_NAME))


class Callback:
    _ctypes_callback_type = ctypes.CFUNCTYPE(None, ctypes.POINTER(GLContext), ctypes.c_void_p)

    def __init__(self, callback, ctx=None):
        def wrapped_callback(egs_gl_context_ptr, user_data_ptr):
            assert not user_data_ptr
            egs_gl_context = None
            if egs_gl_context_ptr:
                egs_gl_context = egs_gl_context_ptr[0]
            callback(egs_gl_context)

        # ctypes callback as attribute to prevent early garbage collection
        self._ctypes_callback = Callback._ctypes_callback_type(wrapped_callback)

        self._callback_ref = _callback_plugin.callback_plugin_create_callback(
            ctx,
            self._ctypes_callback,
            ctypes.c_void_p(0))

    @property
    def display_list_elem_ref(self):
        return self._callback_ref

_callback_plugin.callback_plugin_create_callback.argtypes = [ctypes.POINTER(Context), Callback._ctypes_callback_type, ctypes.c_void_p]
_callback_plugin.callback_plugin_create_callback.restype = ctypes.POINTER(DisplayListElem)
