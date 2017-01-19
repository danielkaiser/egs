#ifndef CALLBACK_PLUGIN_H
#define CALLBACK_PLUGIN_H

/** Callback Plugin for executing generic functions at draw time
 *
 * This plugin allows the user to register a callback function, so that it
 * will be called whenever the corresponding display list is executed. This can
 * be used by applications for the ad-hoc creation of custom display list
 * items, without the need to create a complete plugin.
 *
 * Note: This plugin offers no memory management. In addition to the callback,
 * a void pointer can be passed to callback_plugin_create_callback. When the
 * callback will be called, this user data pointer will be used as second
 * parameter (following the egs_gl_context_ref). It is your responsibility to
 * free this pointer once it is no longer needed.
 */

#include "c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void callback_plugin_init_plugin(egs_context_ref ctx);
void callback_plugin_terminate_plugin();
egs_display_list_elem_ref callback_plugin_create_callback(egs_context_ref ctx, void (*callback)(egs_gl_context_ref ctx, void *user_data), void *user_data);

#ifdef __cplusplus
}
#endif

#endif
