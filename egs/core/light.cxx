#include "light.h"
#include "c_api.h"
#include "context.hxx"

egs_directional_light_t egs_context_get_directional_light_property(egs_context_ref ctx, const char *name) {
  return reinterpret_cast<Context*>(ctx)->get_property<egs_directional_light>(name);
}