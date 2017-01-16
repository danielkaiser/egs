import ctypes
import sys
import os
from sys import platform as _platform

from egs import DisplayListElem, Context

LIB_SUFFIX = 'so'
if _platform == "darwin":
    LIB_SUFFIX = 'dylib'

LIB_NAME = 'libcylinder_plugin.'+LIB_SUFFIX

_cylinder_plugin = ctypes.cdll.LoadLibrary(os.path.join(os.environ.get('EGS_PATH'), 'plugins/cylinder_plugin/', LIB_NAME))


class Cylinder:
    def __init__(self, start, end, radii, color, ctx=None):
        flat_start = [item for position in start for item in position]
        flat_end = [item for position in end for item in position]
        self._cylinder_ref = _cylinder_plugin.cylinder_plugin_create_cylinder(
            ctx,
            ctypes.c_uint(len(start)),
            (ctypes.c_float * len(flat_start))(*flat_start),
            (ctypes.c_float * len(flat_end))(*flat_end),
            (ctypes.c_float * len(radii))(*radii),
            ctypes.c_long(color))

    @property
    def display_list_elem_ref(self):
        return self._cylinder_ref

_cylinder_plugin.cylinder_plugin_create_cylinder.argtypes = [ctypes.POINTER(Context), ctypes.c_uint, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.c_long]
_cylinder_plugin.cylinder_plugin_create_cylinder.restype = ctypes.POINTER(DisplayListElem)
