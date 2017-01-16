//
//  triangle_plugin.h
//  egs
//
//  Created by Daniel Kaiser on 18/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef PLOT_PLUGIN_H
#define PLOT_PLUGIN_H

#include "egs.h"

#ifdef __cplusplus
extern "C" {
#endif

void plot_plugin_init_plugin(egs_context_ref ctx);
void plot_plugin_terminate_plugin();
egs_display_list_elem_ref plot_plugin_create_surface(egs_context_ref ctx, float *positions);

#ifdef __cplusplus
}
#endif

#endif /* triangle_plugin_h */
