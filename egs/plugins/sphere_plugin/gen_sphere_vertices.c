#include "gen_sphere_vertices.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define g 1.618033988749895
#define INDEX(i,j,k) ((i) * 12 + (j)*2 + (k))

int number_of_vertices(int subdivision_level) {
    int i, sum;
    sum = 0;
    for (i=0; i<subdivision_level; i++) {
        sum += 20 * pow(4, i);
    }
    return 1.5 * sum + 12;
}

int number_of_elements(int subdivision_level) {
    return 20 * pow(4, subdivision_level);
}

void calculate_center(GLfloat *v1, GLfloat *v2, GLfloat *center) {
    int i;
    for (i=0; i<3; i++) {
        center[i] = v1[i] + v2[i];
        center[i] /= 2;
    }
}

static void normalize(GLfloat *vec) {
    int i;
    float norm = 0.0;

    for (i=0; i<3; i++) {
        norm += vec[i] * vec[i];
    }
    norm = sqrt(norm);
    for (i=0; i<3; i++) {
        vec[i] /= norm;
    }
}

GLuint add_center(GLuint index1, GLuint index2, GLfloat *vertices, GLuint *refinement_indices, int *next_vertex_index) {
    int j, k;
    GLuint tmp;
    GLfloat tmp_center[3];

    /* index1 < index2 */
    if (index2 < index1) {
        tmp = index1;
        index1 = index2;
        index2 = tmp;
    }
    j = 0;
    /* check if center already is in the refinement list */
    while ((refinement_indices[INDEX(index1, j, 0)] != index2) && (refinement_indices[INDEX(index1, j, 0)] != 0)) {
        j++;
    }
    if (refinement_indices[INDEX(index1, j, 0)] == index2) {
        /* center already inserted */
        return refinement_indices[INDEX(index1, j, 1)];
    } else {
        calculate_center(&(vertices[index1*3]), &(vertices[index2*3]), tmp_center);
        normalize(tmp_center);
        for (k=0; k<3; k++) {
            vertices[(*next_vertex_index)*3 + k] = tmp_center[k];
        }
        refinement_indices[INDEX(index1, j, 0)] = index2;
        refinement_indices[INDEX(index1, j, 1)] = (*next_vertex_index);
        return (*next_vertex_index)++;
    }
}

void create_sphere_vertices(int subdivision_level, int *n_vert, GLfloat **vertices, int *n_elem, GLuint **indices) {
    int i, j, next_vertex_index, n_vertices, n_elements;
    GLuint index1, index2, index3, i12, i13, i23;
    GLuint *refinement_indices;
  
  GLfloat icosahedron_vertices[36] = {
    -1, 0, g,
    1, 0, g,
    -1, 0, -g,
    1, 0, -g,
    0, g, 1,
    0, g, -1,
    0, -g, 1,
    0, -g, -1,
    g, 1, 0,
    -g, 1, 0,
    g, -1, 0,
    -g, -1, 0
  };
  
  GLuint icosahedron_elements[60] = {
    1, 4, 0,
    4, 9, 0,
    4, 5, 9,
    8, 5, 4,
    1, 8, 4,
    1, 10, 8,
    10, 3, 8,
    8, 3, 5,
    3, 2, 5,
    3, 7, 2,
    3, 10, 7,
    10, 6, 7,
    6, 11, 7,
    6, 0, 11,
    6, 1, 0,
    10, 1, 6,
    11, 0, 9,
    2, 11, 9,
    5, 2, 9,
    11, 2, 7
  };

    n_elements = number_of_elements(subdivision_level);
    *n_elem = n_elements;
    n_vertices = number_of_vertices(subdivision_level);
    *n_vert = n_vertices;
    *vertices = (GLfloat *) malloc (n_vertices * 3 * sizeof(GLfloat));
    *indices = (GLuint *) malloc (n_elements * 3 * sizeof(GLuint));

    refinement_indices = (GLuint *) calloc (n_vertices * 6 * 2, sizeof(GLuint));

    /* copy vertices to vertices array */
    for (i=0; i<3*12; i++) {
        (*vertices)[i] = icosahedron_vertices[i];
    }
    /* normalize vertices */
    for (i=0; i<12; i++) {
        normalize( &(*vertices)[3*i]);
    }
    /* copy indices to indices array */
    for (i=0; i<60; i++) {
        (*indices)[i] = icosahedron_elements[i];
    }

    /* refinement loop */
    for (i=1; i<=subdivision_level; i++) {
        next_vertex_index = number_of_vertices(i-1);
        /* refine step */
        /* run backwards to change indices array in place */
        for (j=number_of_elements(i-1)-1; j>=0; j--) {
            index1 = (*indices)[j*3];
            index2 = (*indices)[j*3 + 1];
            index3 = (*indices)[j*3 + 2];

            /* calculate center and add vertex to arrays */
            /* i1 - i2 */
            i12 = add_center(index1, index2, *vertices, refinement_indices, &next_vertex_index);
            /* i1 - i3 */
            i13 = add_center(index1, index3, *vertices, refinement_indices, &next_vertex_index);
            /* i2 - i3 */
            i23 = add_center(index2, index3, *vertices, refinement_indices, &next_vertex_index);
            /* add four triangles */
            /* i1, i12, i13 */
            (*indices)[(j*4+3)*3 + 0] = index1;
            (*indices)[(j*4+3)*3 + 1] = i12;
            (*indices)[(j*4+3)*3 + 2] = i13;
            /* i2, i23, i12 */
            (*indices)[(j*4+2)*3 + 0] = index2;
            (*indices)[(j*4+2)*3 + 1] = i23;
            (*indices)[(j*4+2)*3 + 2] = i12;
            /* i3, i13, i23 */
            (*indices)[(j*4+1)*3 + 0] = index3;
            (*indices)[(j*4+1)*3 + 1] = i13;
            (*indices)[(j*4+1)*3 + 2] = i23;
            /* i13, i12, i23 */
            (*indices)[(j*4+0)*3 + 0] = i13;
            (*indices)[(j*4+0)*3 + 1] = i12;
            (*indices)[(j*4+0)*3 + 2] = i23;
        }
        for (j=0; j<(number_of_vertices(i-1)-1)*6*2; j++) {
            refinement_indices[j] = 0;
        }
    }
    free (refinement_indices);
}


