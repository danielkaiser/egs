#ifndef MOLECULE_PLUGIN_H
#define MOLECULE_PLUGIN_H
#include "c_api.h"

static egs_display_list_elem_ref molecule_plugin_create_molecule(egs_context_ref ctx, const char *filename) {
  return egs_context_get_plugin_func(ctx, "molecule_plugin", "from_xyz_file")(ctx, filename);
}
#endif
