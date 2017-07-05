import egs
from egs.plugins.molecule_plugin import Molecule

context = egs.Context()
glfw_contexts = [
    egs.GLIPGLFWContext(context, [200, 200], [200, 100]),
    egs.GLIPGLFWContext(context, [200, 200], [200, 300]),
    egs.GLIPGLFWContext(context, [200, 200], [200, 500]),
    egs.GLIPGLFWContext(context, [600, 600], [400, 100])
]
display_list = egs.DisplayList()

h2o = Molecule("h2o.xyz")
display_list.add(h2o)

context.look_at([0,0,3], [0,0,0], [0,1,0])
glfw_contexts[1].look_at([0,-3,0], [0,0,0], [0,0,1])
glfw_contexts[2].look_at([-3,0,0], [0,0,0], [0,1,0])

while glfw_contexts:
    for glfw_context in glfw_contexts[:]:
        if not glfw_context.update(display_list):
            glfw_contexts.remove(glfw_context)
            del glfw_context

