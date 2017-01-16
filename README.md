# Extendable Graphics System (EGS)

[![license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## Introduction

The **Extendable Graphics System** (EGS) is a library for rendering two- and three-dimensional graphics using OpenGL. EGS has a plugin-based architecture and can be used from C, C++ or Python.

EGS is available under a [MIT license](LICENSE).

## Installation

### Requirements

 - CMake 3.1.3+
 - OpenGL 3.3+
 - Python 2.7+ (libpython and Python.h)
 - Numpy
    
### Compiling

```
git clone "https://github.com/DanielKaiser/egs"
cd egs
make EGS_INSTALL_PREFIX=/usr/local/
make install
```

## Getting Started 

In order to use **EGS**, you need to perform the following steps:
1. Include EGS core
2. Include EGS plugins to create drawable objects
3. Create an EGS context
4. Create a rendering context (e.g. a window)
5. Create an EGS Display List and add drawable objects to it
6. Update the rendering context using the EGS Display List

### 1. Include EGS core
To render an image using **EGS** you need to include the **EGS** core which provides the basic data structures and functions:

| Language | Code |
|----------|------|
| C/C++    | `#include "egs.h"` |
| Python   | `import egs` |


### 2. Include EGS plugins
Drawable elements, like spheres and molecules, are contained in plugins, so you need to include at least one plugin to draw elements. These plugins can be implemented either in C++, C or Python. Additionally they should provide wrappers to call the plugin functions from other programming languages. For instance a C++ plugin should contain a C and Python wrapper. Of course, C plugins can also be used in C++.

Each plugin should have a `<plugin_name>.py` for Python, a `<plugin_name>.h` for C and there might also be a `<plugin_name>.hxx` file for C++. For example, the molecule plugin which does not have a hxx file can be included like this:

| Language | Code |
|----------|------|
| C/C++    | `#include "molecule_plugin.h"` |
| Python   | `from molecule_plugin import Molecule` |

### 3. Create an EGS context

The EGS context contains the settings used for rendering scenes, such as the background color and camera position. The previously included plugins are loaded dynamically at the runtime of the application. Therefore a plugin loader is needed, which is also one of the tasks of the EGS context. The following code creates an EGS context:

| Language | Code |
|----------|------|
| C++      | `Context ctx;` |
| C        | `egs_context_ref ctx = egs_context_create();` |
| Python   | `ctx = egs.Context()` |

### 4. Create a rendering context
The actual rendering is done using rendering context, so you need to create at least one of these to render a scene. They are also used to override the general settings provided by the EGS context, so that you can change the camera position in one window for example. EGS contains the following types of rendering contexts:

#### GLFW based context
This type of rendering context creates a window using the [GLFW library](http://www.glfw.org/) to display the scene. These contexts will only process the window events when you call their update method. Therefore they are only suitable if your application does this frequently, otherwise the operating system will report the application as unresponsive and may shut it down.

To create an GLFW based context, you can use this code:

| Language | Code |
|----------|------|
| C++      | `GLFWContext gl_ctx = GLFWContext(ctx);` |
| C        | `egs_gl_context_ref gl_ctx = (egs_gl_context_ref)egs_glfw_context_create(ctx);` |
| Python   | `gl_ctx = egs.GLFWContext(ctx)` |

**Note**: In the interactive Python shell, use the next type of context instead to avoid the application from becoming unresponsive.


#### GLFW based context using the GLIP library
If you cannot ensure that the rendering context will be updated frequently (roughly at least once in a second), you should use a GLFW based context which uses the GLIP library. This will create a window using the GLFW library, but in a separate process which handles events. 

If this context gets updated, the OpenGL commands executed in the main application are sent to the rendering process using the Open**GL** **I**nter **P**rocess library (GLIP) which is part of EGS.

To create an GLFW based context in a separate process using GLIP, you can use this code:

| Language | Code |
|----------|------|
| C++      | `GLIPGLFWContext gl_ctx = GLIPGLFWContext(ctx);` |
| C        | `egs_gl_context_ref gl_ctx = (egs_gl_context_ref)egs_glip_glfw_context_create(ctx);` |
| Python   | `gl_ctx = egs.GLIPGLFWContext(ctx)` |


**Note**: Currently GLIP does not send the window events to the main application, so you cannot interact with the scene.

#### Offscreen rendering
EGS also contains a rendering context which draws into a texture and does not create a window. This can for example be used to create png files or to display the scene in a different application.

To create an offscreen context, you can use this code:


| Language | Code |
|----------|------|
| C++      | `GLOffscreenRenderer gl_ctx = GLOffscreenRenderer(ctx);` |
| C        | `egs_gl_context_ref gl_ctx = (egs_gl_context_ref)egs_gloffscreen_context_create(ctx);` |
| Python   | `gl_ctx = egs.GLOffscreenRenderer(ctx)` |

### 5. Create a Display List and add drawable objects to it
Display lists are sequences of drawable objects created using EGS plugins. They are passed to the update method of a rendering context, which then draws all elements in the list. To create a display list use the following code:

| Language | Code |
|----------|------|
| C++      | `auto display_list = std::make_shared<DisplayList>();` |
| C        | `egs_display_list_ref display_list = egs_display_list_create();` |
| Python   | `display_list = egs.DisplayList()` |

Now you can add drawable elements like this:

| Language | Code |
|----------|------|
| C++      | `display_list->add(molecule_plugin_create_molecule(ctx, "h2o.xyz"));` |
| C        | `egs_display_list_add_element(display_list, molecule_plugin_create_molecule(ctx, "h2o.xyz"));` |
| Python   | `display_list.add(Molecule("h2o.xyz"))` |

### 6. Update the rendering context using the Display List
Finally you can draw the elements of a display list on a rendering context by calling its update method:


| Language | Code |
|----------|------|
| C++      | `gl_ctx.update(display_list);` |
| C        | `egs_gl_context_update(gl_ctx, display_list);` |
| Python   | `gl_context.update(display_list)` |

## Example
The following examples show how to render a water molecule using the **EGS** and its molecule plugin in Python, C++ and C. The source code and the xyz file used in these examples is located in the examples folder of EGS and the binary files will be installed to `${INSTALL_PREFIX}/bin/egs/examples`.

The molecule geometry is described in the file `h2o.xyz` which has the following content:

```
3
Water molecule
H 0.757 0.586 0.000
H -0.757 0.586 0.000
O 0.000 0.000 0.000
```

### Python

```python
import egs
from molecule_plugin import Molecule

context = egs.Context()
glfw_context = egs.GLIPGLFWContext(context)
display_list = egs.DisplayList()

h2o = Molecule("h2o.xyz")
display_list.add(h2o)

while glfw_context.update(display_list):
    context.rotate([0, 1, 0], 0.01)
```

### C

```C
#include "egs.h"
#include "molecule_plugin.h"

int main(void) {
  egs_context_ref ctx = egs_context_create();
  egs_gl_context_ref glfw_ctx = (egs_gl_context_ref)egs_glfw_context_create(ctx);
  egs_display_list_ref display_list = egs_display_list_create();

  egs_context_load_plugin(ctx, "molecule_plugin");
  egs_display_list_add_element(display_list, molecule_plugin_create_molecule(ctx, "h2o.xyz"));
  
  while (egs_gl_context_update(glfw_ctx, display_list));
  
  egs_display_list_destroy(display_list);
  egs_glip_glfw_context_destroy((egs_glip_glfw_context_ref)glfw_ctx);
  egs_context_destroy(ctx);
  return 0;
}
```

### C++

```C++
#include "egs.h"
#include "molecule_plugin.h"

int main(void) {
  Context ctx;
  GLFWContext glfw_ctx = GLFWContext(ctx);
  auto display_list = std::make_shared<DisplayList>();

  display_list->add(molecule_plugin_create_molecule(ctx, "h2o.xyz"));

  while (glfw_ctx.update(display_list));

  return 0;
}
```

