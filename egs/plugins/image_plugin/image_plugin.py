from __future__ import print_function

import ctypes
import sys
import os
import numpy as np

lib_path = os.path.abspath(os.path.join('..', '..'))
sys.path.append(lib_path)
from egs import DisplayListElem, PluginWrapper, Context, SerializedDisplayListElem
import egs

try:
    import PIL.Image
except ImportError:
    egs.printf("Could not import PIL.", egs.WARNING)
import egs.glip as OpenGL


class Image:
    apply_fun = None
    plugin_name = ctypes.c_char_p("image_plugin")
    plugin_instances = {}
    plugin_instance_ctr = 0

    def __init__(self, width, height, rgb_data):
        self._data = PluginWrapper()
        self._data.plugin_name = Image.plugin_name
        self.width = width
        self.height = height
        self.initialised = False
        self.rgb_data = np.array(rgb_data, dtype=np.uint8)

        self._data.data_length = ctypes.sizeof(ctypes.c_uint32)
        self._data.data = (ctypes.c_uint8 * self._data.data_length)(Image.plugin_instance_ctr)
        Image.plugin_instances[Image.plugin_instance_ctr] = self
        Image.plugin_instance_ctr += 1
        self._image_ref = self._data.get_display_list_elem()
        egs.printf("created image", egs.DEBUG)

    @staticmethod
    def from_image_file(filename):
        try:
            im = PIL.Image.open(filename)
        except NameError:
            egs.printf("PIL module not loaded.", egs.WARNING)
            return Image(width=0, height=0, rgb_data=[])
        w, h = im.size
        rgba = list(im.getdata())
        im.close()
        rgb = []
        for elem in rgba:
            if len(elem) == 3:
                rgb += [elem[0], elem[1], elem[2], 255]
            else:
                rgb += [elem[0], elem[1], elem[2], elem[3]]
        return Image(width=w, height=h, rgb_data=rgb)

    @staticmethod
    def from_image_file_c_wrapper(_ctx, filename):
        return ctypes.addressof(Image.from_image_file(filename).display_list_elem_ref.contents)


    @staticmethod
    def terminate(data_size, data, plugin_data):
        egs.printf("image destructor", egs.DEBUG)

    @property
    def display_list_elem_ref(self):
        return self._image_ref

    @staticmethod
    def apply(ctx_ref, data_size, data, plugin_data):
        instance = ctypes.cast(data, ctypes.POINTER(ctypes.c_uint32))[0]
        if instance not in Image.plugin_instances:
            egs.printf("Plugin instance not found", egs.WARNING)
        else:
            if not Image.plugin_instances[instance].initialised:
                Image.plugin_instances[instance].texture = OpenGL.glGenTextures(1)
                OpenGL.glBindTexture(OpenGL.GL_TEXTURE_2D, Image.plugin_instances[instance].texture)
                OpenGL.glTexParameterf(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_S, OpenGL.GL_REPEAT)
                OpenGL.glTexParameterf(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_T, OpenGL.GL_CLAMP_TO_EDGE)
                OpenGL.glTexParameteri(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MAG_FILTER, OpenGL.GL_LINEAR)
                OpenGL.glTexParameteri(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MIN_FILTER, OpenGL.GL_LINEAR_MIPMAP_LINEAR)
                width = Image.plugin_instances[instance].width
                height = Image.plugin_instances[instance].height
                OpenGL.glTexImage2D(OpenGL.GL_TEXTURE_2D, 0, OpenGL.GL_RGB, width, height, 0, OpenGL.GL_RGBA,
                                    OpenGL.GL_UNSIGNED_BYTE, Image.plugin_instances[instance].rgb_data)
                OpenGL.glGenerateMipmap(OpenGL.GL_TEXTURE_2D)
                Image.plugin_instances[instance].initialised = True

            OpenGL.glActiveTexture(OpenGL.GL_TEXTURE0)
            OpenGL.glBindTexture(OpenGL.GL_TEXTURE_2D, Image.plugin_instances[instance].texture)


image_create_fun = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.POINTER(Context), ctypes.c_char_p)
wrapped_image_create_fun = image_create_fun(Image.from_image_file_c_wrapper)


def image_plugin_init_plugin(ctx_ref):
    Image.apply_fun = PluginWrapper.apply_fun_callback(Image.apply)
    Image.terminate_fun = PluginWrapper.terminate_fun_callback(Image.terminate)
    PluginWrapper.register_c_plugin(Image.plugin_name, Image.apply_fun, Image.terminate_fun)

    Context.register_py_plugin_function(ctx_ref, Image.plugin_name, Image.from_image_file.__name__,
                                        wrapped_image_create_fun)
    return True


def image_plugin_terminate_plugin():
    egs.printf("Image plugin terminated")
