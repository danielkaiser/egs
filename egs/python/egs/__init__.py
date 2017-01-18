import ctypes
import os
import sys
import importlib
from sys import platform as _platform
import warnings


if not os.environ.get('EGS_PATH'):
    os.environ['EGS_PATH'] = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

os.environ["DYLD_LIBRARY_PATH"] = os.environ.get('DYLD_LIBRARY_PATH', '') + ":" + os.path.abspath("../")
os.environ["LD_LIBRARY_PATH"] = os.environ.get('LD_LIBRARY_PATH', '') + ":" + os.path.abspath("../")

_plugins = os.listdir(os.path.join(os.environ['EGS_PATH'], 'plugins'))
for plugin in _plugins:
    plugin_path = os.path.join(os.environ['EGS_PATH'], 'plugins', plugin)
    if os.path.isdir(plugin_path):
        sys.path.append(plugin_path)

LIB_SUFFIX = 'so'
if _platform == "darwin":
    LIB_SUFFIX = 'dylib'

LIB_NAME = 'libegs.'+LIB_SUFFIX

_egs = ctypes.CDLL(os.path.join(os.environ.get('EGS_PATH'), LIB_NAME))


(DEBUG, WARNING, ERROR) = map(ctypes.c_uint, range(1, 4))


def _py_loader_callback(ctx, path, plugin_name):
    if path:
        path_already_in_sys_path = (path in sys.path)
        if not path_already_in_sys_path:
            sys.path.append(path)
        try:
            module = importlib.import_module(plugin_name)
            getattr(module, plugin_name + '_init_plugin')(ctx)
            _py_loader_callback.imported_modules[plugin_name] = module
        except (ImportError, AttributeError) as e:
            warnings.warn("Plugin {} could not be loaded!".format(plugin_name), 'error')
        finally:
            if not path_already_in_sys_path:
                sys.path.remove(path)
    else:
        try:
            getattr(_py_loader_callback.imported_modules[plugin_name], plugin_name + '_terminate_plugin')()
        except (KeyError, AttributeError) as e:
            warnings.warn("Plugin {} could not be unloaded!".format(plugin_name), 'error')
_py_loader_callback.imported_modules = {}


def printf(msg, msg_type=DEBUG, end=b"\n"):
    _egs.egs_printf(msg_type, msg+end)


class DisplayListElem(ctypes.Structure):
    _fields_ = []


class SerializedDisplayListElem(ctypes.Structure):
    _fields_ = [("data_length", ctypes.c_size_t), ("data", ctypes.POINTER(ctypes.c_uint8))]


class DisplayList(ctypes.Structure):
    _fields_ = []

    def __init__(self, dl_ref=None):
        super(DisplayList, self).__init__()
        self._dl_ref = _egs.egs_display_list_create()

    def __del__(self):
        if self._dl_ref:
            _egs.egs_display_list_destroy(self._dl_ref)

    def add(self, dl_elem):
        _egs.egs_display_list_add_element(self._dl_ref, dl_elem.display_list_elem_ref)

    @property
    def display_list_ref(self):
        return self._dl_ref


class Context(ctypes.Structure):
    _fields_ = []

    def __init__(self):
        super(Context, self).__init__()
        self._ctx_ref = _egs.egs_context_create()
        _egs.egs_context_set_py_loader_fun(self._ctx_ref, _wrapped_loader_fun)

    def __del__(self):
        if self._ctx_ref:
            _egs.egs_context_destroy(self._ctx_ref)

    def load_plugin(self, plugin_name):
        _egs.egs_context_load_plugin(ctypes.cast(self._ctx_ref, ctypes.POINTER(Context)), plugin_name)

    def rotate(self, axis, angle):
        _egs.egs_context_rotate(self._ctx_ref, axis[0], axis[1], axis[2], angle)

    @staticmethod
    def load_plugin_ptr(ctx_ref, plugin_name):
        _egs.egs_context_load_plugin(ctypes.cast(ctx_ref, ctypes.POINTER(Context)), plugin_name)

    def set_mat4_property(self, name, value):
        flat_value = [item for position in value for item in position]
        _egs.egs_context_set_property(self._ctx_ref, ctypes.c_char_p(name.encode('utf-8')), ctypes.cast((ctypes.c_float * len(flat_value))(*flat_value), ctypes.c_void_p), 16*ctypes.sizeof(ctypes.c_cfloat))

    def get_mat4_property(self, name):
        res = _egs.context_get_mat4_property(self._ctx_ref, ctypes.c_char_p(name.encode('utf-8')))
        arr = []
        for i in range(4):
            tmp = []
            for j in range(4):
                tmp.append(res[i*4+j])
            arr.append(tmp)
        return arr

    def set_vec3_property(self, name, value):
        _egs.egs_context_set_property(self._ctx_ref, ctypes.c_char_p(name.encode('utf-8')),
                                       ctypes.cast((ctypes.c_float * len(value))(*value), ctypes.c_void_p), 3*ctypes.sizeof(ctypes.c_float))

    @staticmethod
    def register_py_plugin_function(ctx_ref, plugin_name, function_name, function):
        if type(ctx_ref) != ctypes.POINTER(Context):
            ctx_ref = ctypes.cast(ctx_ref, ctypes.POINTER(Context))
        _egs.egs_context_register_py_plugin_function(ctx_ref, plugin_name, function_name,
                                                             ctypes.cast(function, ctypes.c_void_p))

    @staticmethod
    def call_apply(ctx_ref, dl_elem):
        _egs.egs_display_list_element_apply(dl_elem, ctx_ref)

    @staticmethod
    def call_terminate(dl_elem):
        _egs.egs_display_list_element_terminate(dl_elem)

    @property
    def context_ref(self):
        return self._ctx_ref


class GLContext(ctypes.Structure):
    _fields_ = []

    def set_vec4_property(self, name, value):
        _egs.egs_gl_context_set_property(self._gl_ctx_ref, ctypes.c_char_p(name.encode('utf-8')),
                                       ctypes.cast((ctypes.c_float * len(value))(*value), ctypes.c_void_p), 4*ctypes.sizeof(ctypes.c_float))

    def update(self, display_list):
        return _egs.egs_gl_context_update(self._gl_ctx_ref, display_list.display_list_ref)


class GLFWContext(GLContext, ctypes.Structure):
    _fields_ = []

    def __init__(self, context):
        super(GLFWContext, self).__init__()
        self._gl_ctx_ref = _egs.egs_glfw_context_create(context.context_ref)

    def __del__(self):
        if self._gl_ctx_ref:
            _egs.egs_glfw_context_destroy(self._gl_ctx_ref)


class GLIPGLFWContext(GLContext, ctypes.Structure):
    _fields_ = []

    def __init__(self, context):
        super(GLIPGLFWContext, self).__init__()
        self._gl_ctx_ref = _egs.egs_glip_glfw_context_create(context.context_ref)

    def __del__(self):
        if self._gl_ctx_ref:
            _egs.egs_glip_glfw_context_destroy(self._gl_ctx_ref)


class GLOffscreenRenderer(GLContext, ctypes.Structure):
    _fields_ = []

    def __init__(self, context):
        super(GLOffscreenRenderer, self).__init__()
        self._gl_ctx_ref = _egs.egs_gloffscreen_context_create(context.context_ref)

    def __del__(self):
        if self._gl_ctx_ref:
            _egs.egs_gloffscreen_context_destroy(self._gl_ctx_ref)

    def get_pixel_data(self):
        return _egs.egs_gloffscreen_get_data(self._gl_ctx_ref)


def _empty_destructor(data_len, data, plugin_data):
    pass


class PluginWrapper(ctypes.Structure):
    _fields_ = [("plugin_name", ctypes.c_char_p), ("data_length", ctypes.c_size_t), ("data", ctypes.POINTER(ctypes.c_uint8))]
    apply_fun_callback = ctypes.CFUNCTYPE(None, ctypes.POINTER(GLContext), ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint8), ctypes.c_void_p)
    terminate_fun_callback = ctypes.CFUNCTYPE(None, ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint8), ctypes.c_void_p)
    empty_terminate_fun = terminate_fun_callback(_empty_destructor)

    def get_display_list_elem(self):
        return _egs.egs_c_wrapper_create(self)

    @staticmethod
    def register_c_plugin(name_ptr, apply_fun, terminate_fun=None):
        if not terminate_fun:
            terminate_fun = PluginWrapper.empty_terminate_fun
        _egs.egs_c_wrapper_register_c_plugin(name_ptr, apply_fun, terminate_fun)


_egs.egs_context_create.argtypes = []
_egs.egs_context_create.restype = ctypes.POINTER(Context)
_egs.egs_context_destroy.argtypes = [ctypes.POINTER(Context)]
_egs.egs_context_destroy.restype = None
_egs.egs_context_load_plugin.argtypes = [ctypes.POINTER(Context), ctypes.c_char_p]
_egs.egs_context_load_plugin.restype = None

_egs.egs_display_list_create.argtypes = []
_egs.egs_display_list_create.restype = ctypes.POINTER(DisplayList)
_egs.egs_display_list_add_element.argtypes = [ctypes.POINTER(DisplayList), ctypes.POINTER(DisplayListElem)]
_egs.egs_display_list_add_element.restype = None
_egs.egs_display_list_element_apply.argtypes = [ctypes.POINTER(DisplayListElem), ctypes.POINTER(GLContext)]
_egs.egs_display_list_element_apply.restype = None
_egs.egs_display_list_destroy.argtypes = [ctypes.POINTER(DisplayList)]
_egs.egs_display_list_destroy.restype = None

_egs.egs_glfw_context_create.argtypes = [ctypes.POINTER(Context)]
_egs.egs_glfw_context_create.restype = ctypes.POINTER(GLFWContext)
_egs.egs_glfw_context_destroy.argtypes = [ctypes.POINTER(GLFWContext)]
_egs.egs_glfw_context_destroy.restype = None

_egs.egs_glip_glfw_context_create.argtypes = [ctypes.POINTER(Context)]
_egs.egs_glip_glfw_context_create.restype = ctypes.POINTER(GLIPGLFWContext)
_egs.egs_glip_glfw_context_destroy.argtypes = [ctypes.POINTER(GLIPGLFWContext)]
_egs.egs_glip_glfw_context_destroy.restype = None

_egs.egs_gloffscreen_context_create.argtypes = [ctypes.POINTER(Context)]
_egs.egs_gloffscreen_context_create.restype = ctypes.POINTER(GLOffscreenRenderer)
_egs.egs_gloffscreen_context_destroy.argtypes = [ctypes.POINTER(GLOffscreenRenderer)]
_egs.egs_gloffscreen_context_destroy.restype = None
_egs.egs_gloffscreen_get_data.argtypes = []
_egs.egs_gloffscreen_get_data.restype = ctypes.POINTER(ctypes.c_int)

_egs.egs_c_wrapper_create.argtypes = [PluginWrapper]
_egs.egs_c_wrapper_create.restype = ctypes.POINTER(DisplayListElem)
_egs.egs_c_wrapper_register_c_plugin.argtypes = [ctypes.c_char_p, PluginWrapper.apply_fun_callback, PluginWrapper.terminate_fun_callback]
_egs.egs_c_wrapper_register_c_plugin.restype = None

_egs_py_loader_fun = ctypes.CFUNCTYPE(None, ctypes.POINTER(GLContext), ctypes.c_char_p, ctypes.c_char_p)
_wrapped_loader_fun = _egs_py_loader_fun(_py_loader_callback)
_egs.egs_context_set_py_loader_fun.argtypes = [ctypes.POINTER(Context), _egs_py_loader_fun]
_egs.egs_context_set_py_loader_fun.restype = None
_egs.egs_context_rotate.argtypes = [ctypes.POINTER(Context), ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float]
_egs.egs_context_rotate.restype = None
_egs.egs_context_set_property.argtypes = [ctypes.POINTER(Context), ctypes.c_char_p, ctypes.c_void_p, ctypes.c_size_t]
_egs.egs_context_set_property.restype = None
_egs.egs_context_get_property.argtypes = [ctypes.POINTER(Context), ctypes.c_char_p, ctypes.c_void_p, ctypes.c_size_t]
_egs.egs_context_get_property.restype = ctypes.c_void_p
_egs.egs_context_register_py_plugin_function.argtypes = [ctypes.POINTER(Context), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_void_p]
_egs.egs_context_register_py_plugin_function.restype = None

_egs.egs_gl_context_update.argtypes = [ctypes.POINTER(GLContext), ctypes.POINTER(DisplayList)]
_egs.egs_gl_context_update.restype = ctypes.c_int

_egs.egs_printf.argtypes = [ctypes.c_uint, ctypes.c_char_p]
_egs.egs_printf.restype = None
