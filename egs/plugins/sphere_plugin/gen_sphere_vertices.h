#ifdef __MACH__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

void create_sphere_vertices(int subdivision_level, int *n_vert, GLfloat **vertices, int *n_elem, GLuint **indices);
int number_of_elements(int subdivision_level);

