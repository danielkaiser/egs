import ctypes
import sys
import os
from egs import DisplayListElem, Context

_rendertotexture_plugin = ctypes.cdll.LoadLibrary(os.path.join(os.environ.get('EGS_PATH'), 'plugins/rendertotexture_plugin/', 'librendertotexture_plugin.so'))


class Surface:
    def __init__(self, positions):
        flat_positions = [item for position in positions[:] for item in position]
        self._rendertotexture_ref = _rendertotexture_plugin.rendertotexture_plugin_create_surface(
            (ctypes.c_float * len(flat_positions))(*flat_positions))

    #def apply(self, ctx, data_ptr):
    #    _rendertotexture_plugin.surface_apply(ctx, self._sphere_ref.data_length, self._sphere_ref.data, data_ptr)

    @property
    def display_list_elem_ref(self):
        return self._rendertotexture_ref

_rendertotexture_plugin.rendertotexture_plugin_create_surface.argtypes = [ctypes.POINTER(ctypes.c_float)]
_rendertotexture_plugin.rendertotexture_plugin_create_surface.restype = ctypes.POINTER(DisplayListElem)
#_sphere_plugin.sphere_apply.argtypes = [ctypes.POINTER(Context), ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint8), ctypes.c_void_p]
