/*
//  calculate_bonds.c
//  seminar
//
//  Created by Daniel Kaiser on 01/10/15.
//
*/

#include "calculate_bonds.h"
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "egs.h"

#define CELL_IDX(cell, dim) cell.z*dim.x*dim.y+cell.y*dim.x+cell.x

#define EPS 0.001

typedef struct{
  double x;
  double y;
  double z;
} double3;

typedef struct{
  float x;
  float y;
  float z;
} float3;

typedef struct{
  unsigned char x;
  unsigned char y;
  unsigned char z;
} uchar3;

typedef struct {
  int x;
  int y;
  int z;
} int3;

void put_in_cells(float *x, float *y, float* z, double3 min, uchar3 *cells, unsigned int *position_in_cell, unsigned int *atoms_per_cell, double3 cell_size, int3 dim, int n) {
  uchar3 c;
  int i;
  unsigned int cell;
  for (i = 0; i < n; i++) {
    c.x = (int)((x[i]-min.x)/cell_size.x);
    c.y = (int)((y[i]-min.y)/cell_size.y);
    c.z = (int)((z[i]-min.z)/cell_size.z);
    cell = CELL_IDX(c, dim);
    position_in_cell[i] = (*(atoms_per_cell + cell))++;
    cells[i]=c;
  }
}

void sort_atoms(float *x, float *y, float* z, float3 *particles, uchar3 *cells_ordered, uchar3 *cells, int3 dim, unsigned int *cell_offset, unsigned int *position_in_cell, int n) {
  int i;
  int c;
  for (i = 0; i < n; i++) {
    float3 p;
    p.x = x[i];
    p.y = y[i];
    p.z = z[i];
    c = CELL_IDX(cells[i], dim);
    particles[cell_offset[c]+position_in_cell[i]] = p;
    cells_ordered[cell_offset[c]+position_in_cell[i]] = cells[i];
  }
}

unsigned int calculate_bonds(float3 *particles, uchar3* cells, int3 dim, float bond_length, float3 **bond_start, float3 **bond_end, unsigned int *cell_offset, unsigned int n) {
  int ix, iy, iz;
  unsigned int i, ic, c2_index, allocated = 0, number_of_bonds = 0;
  float3 p, p2;
  float d;
  uchar3 c, c2;
  for (i = 0; i < n; i++) {
    p = particles[i];
    c = cells[i];
    for (iz=c.z-1; iz<=c.z+1; iz++) {
      if (iz<0 || iz>=dim.z) continue;
      for (iy=c.y-1; iy<=c.y+1; iy++) {
        if (iy<0 || iy>=dim.y) continue;
        for (ix=c.x-1; ix<=c.x+1; ix++) {
          if (ix<0 || ix>=dim.x) continue;
          c2.x = ix;
          c2.y = iy;
          c2.z = iz;
          c2_index = CELL_IDX(c2, dim);
          for (ic=cell_offset[c2_index]; ic<cell_offset[c2_index+1]; ic++) {
            p2 = particles[ic];
            if(i<=ic) continue;
            d = (p.x-p2.x) * (p.x-p2.x) + (p.y-p2.y) * (p.y-p2.y) + (p.z-p2.z) * (p.z-p2.z);
            if (d + EPS > bond_length)
              continue;
            if (++number_of_bonds >= allocated * n) {
              allocated++;
              *bond_start = realloc(*bond_start, n * allocated * sizeof(float3));
              *bond_end = realloc(*bond_end, n * allocated * sizeof(float3));
              assert(*bond_start);
              assert(*bond_end);
            }
            (*bond_start)[number_of_bonds-1] = p;
            (*bond_end)[number_of_bonds-1] = p2;
          }
        }
      }
    }
  }
  return number_of_bonds;
}


float min(float *values, int n) {
  int i;
  float cur_min;
  cur_min = values[0];
  for (i=0; i<n; i++) {
    if (values[i] < cur_min) {
      cur_min = values[i];
    }
  }
  return cur_min;
}

float max(float *values, int n) {
  int i;
  float cur_max;
  cur_max = values[0];
  for (i=0; i<n; i++) {
    if (values[i] > cur_max) {
      cur_max = values[i];
    }
  }
  return cur_max;
}

int calc_bonds(float *x_pos, float *y_pos, float *z_pos, int num_atoms, float bond_length, float **start, float **end) {
  int3 dim;
  int i, num_cells, num_bonds = 0;
  unsigned int *position_in_cell, *atoms_per_cell, *cell_offset;
  uchar3 *cells_ordered, *cells;
  float3 *particles;
  double3 _min, cell_size;

  assert(num_atoms > 0);
  cells = calloc(num_atoms, sizeof(uchar3));
  _min.x = min(x_pos, num_atoms);
  _min.y = min(y_pos, num_atoms);
  _min.z = min(z_pos, num_atoms);
  dim.x = (max(x_pos, num_atoms) - _min.x) / bond_length + 1;
  dim.y = (max(y_pos, num_atoms) - _min.y) / bond_length + 1;
  dim.z = (max(z_pos, num_atoms) - _min.z) / bond_length + 1;
  num_cells = dim.x * dim.y * dim.z;

  position_in_cell = calloc(num_atoms, sizeof(unsigned int));
  atoms_per_cell = calloc(num_cells, sizeof(unsigned int));

  cell_size.x = bond_length;
  cell_size.y = bond_length;
  cell_size.z = bond_length;

  put_in_cells(x_pos, y_pos, z_pos, _min, cells, position_in_cell, atoms_per_cell, cell_size, dim, num_atoms);

  cell_offset = (unsigned int *)malloc((num_cells + 1) * sizeof(unsigned int));
  cell_offset[0] = 0;
  for (i=1; i <= num_cells; i++) {
    cell_offset[i] = atoms_per_cell[i-1] + cell_offset[i-1];
  }

  cells_ordered = (uchar3 *)malloc(num_atoms * sizeof(uchar3));
  particles = calloc(num_atoms, sizeof(float3));
  sort_atoms(x_pos, y_pos, z_pos, particles, cells_ordered, cells, dim, cell_offset, position_in_cell, num_atoms);

  *start = NULL;
  *end = NULL;
  num_bonds = calculate_bonds(particles, cells_ordered, dim, bond_length * bond_length, (float3 **)start, (float3 **)end, cell_offset, num_atoms);
  egs_printf(EGS_DEBUG, "number of bonds: %d\n",num_bonds);

  free(cells);
  free(position_in_cell);
  free(atoms_per_cell);
  free(cell_offset);
  free(cells_ordered);
  free(particles);

  return num_bonds;
}

