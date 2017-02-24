#ifndef CALLBACK_PLUGIN_HXX
#define CALLBACK_PLUGIN_HXX

/** Callback Plugin for executing generic functions at draw time
 *
 * This plugin allows the user to register a callback function, so that it
 * will be called whenever the corresponding display list is executed. This can
 * be used by applications for the ad-hoc creation of custom display list
 * items, without the need to create a complete plugin.
 *
 * Note: This plugin offers no memory management. The callback will be copied
 * into a new std::function object, so if you use a lambda with a captured 
 * reference, you need to make sure the referred to object is kept alive and
 * deleted as neccessary.
 */

#include <functional>

#include "callback_plugin.h"

#include "displaylist.hxx"
#include "context.hxx"

class Context;

namespace CallbackPlugin {
  class Callback : public IDisplayListElement {
  public:
    typedef std::function<void(GLContext&)> callback_type;
    Callback(const callback_type& callback);
    virtual ~Callback();
    virtual void apply(GLContext& ctx);
    virtual void delete_handler(GLContext& ctx);

  private:
    callback_type m_callback;
  };
}


#endif
