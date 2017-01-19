import egs
from egs.plugins.molecule_plugin import Molecule

context = egs.Context()
glfw_context = egs.GLIPGLFWContext(context)
display_list = egs.DisplayList()

h2o = Molecule("h2o.xyz")
display_list.add(h2o)

while glfw_context.update(display_list):
    context.rotate([0, 1, 0], 0.01)
