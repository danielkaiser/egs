
from __future__ import print_function

import egs
from egs.plugins.callback_plugin import Callback

context = egs.Context()
glfw_context = egs.GLIPGLFWContext(context)
display_list = egs.DisplayList()

display_list.add(Callback(lambda gl_ctx: print("Hello EGS!")))

while glfw_context.update(display_list):
    pass
