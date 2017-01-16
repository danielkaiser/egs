//
//  triangle_plugin.h
//  egs
//
//  Created by Daniel Kaiser on 18/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef TRIANGLE_PLUGIN_H
#define TRIANGLE_PLUGIN_H

#include "egs.h"

#ifdef __cplusplus
extern "C" {
#endif

void triangle_plugin_init_plugin(egs_context_ref ctx);
void triangle_plugin_terminate_plugin();
egs_display_list_elem_ref triangle_plugin_create_triangle(egs_context_ref ctx, int, float*, float*, long);

#ifdef __cplusplus
}
#endif

#endif /* triangle_plugin_h */
