import ctypes
import sys
import os
from sys import platform as _platform
from egs import DisplayListElem, Context

LIB_SUFFIX = 'so'
if _platform == "darwin":
    LIB_SUFFIX = 'dylib'

LIB_NAME = 'libsphere_plugin.'+LIB_SUFFIX

_sphere_plugin = ctypes.cdll.LoadLibrary(os.path.join(os.environ.get('EGS_PATH'), 'plugins/sphere_plugin/', LIB_NAME))


class Sphere:
    def __init__(self, positions, radii, colors, ctx=None):
        flat_positions = [item for position in positions[:] for item in position]
        self._sphere_ref = _sphere_plugin.sphere_plugin_create_spheres(
            ctx,
            ctypes.c_uint(len(radii)),
            (ctypes.c_float * len(flat_positions))(*flat_positions),
            (ctypes.c_float * len(positions))(*radii),
            (ctypes.c_long * len(positions))(*colors))

    @property
    def display_list_elem_ref(self):
        return self._sphere_ref


class TexturedSphere:
    def __init__(self, positions, radii, ctx=None):
        flat_positions = [item for position in positions[:] for item in position]
        self._sphere_ref = _sphere_plugin.sphere_plugin_create_textured_spheres(
            ctx,
            ctypes.c_uint(len(radii)),
            (ctypes.c_float * len(flat_positions))(*flat_positions),
            (ctypes.c_float * len(positions))(*radii))

    @property
    def display_list_elem_ref(self):
        return self._sphere_ref

_sphere_plugin.sphere_plugin_create_spheres.argtypes = [ctypes.POINTER(Context), ctypes.c_uint, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_long)]
_sphere_plugin.sphere_plugin_create_spheres.restype = ctypes.POINTER(DisplayListElem)

_sphere_plugin.sphere_plugin_create_textured_spheres.argtypes = [ctypes.POINTER(Context), ctypes.c_uint, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]
_sphere_plugin.sphere_plugin_create_textured_spheres.restype = ctypes.POINTER(DisplayListElem)