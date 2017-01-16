//
//  triangle_plugin.h
//  egs
//
//  Created by Daniel Kaiser on 18/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef ISOSURFACE_PLUGIN_H
#define ISOSURFACE_PLUGIN_H

#include "c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void isosurface_plugin_init_plugin(egs_context_ref ctx);
void isosurface_plugin_terminate_plugin();
//display_list_elem_ref triangle_plugin_create_triangle(int, float*, float*, long);

#ifdef __cplusplus
}
#endif

#endif /* triangle_plugin_h */
