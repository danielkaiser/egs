#ifndef GLIP_EVENT_LIST_H
#define GLIP_EVENT_LIST_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum {
    GLIP_EVENT_KEY = 1,
    GLIP_EVENT_MOUSE = 2,
    GLIP_EVENT_SCROLL = 3,
    GLIP_EVENT_WINDOW_SIZE = 4
  };

  typedef struct {
    size_t current_length;
    unsigned char *data;
  } event_list_t;

  event_list_t *event_list_create();
  event_list_t *event_list_create_from_char_ptr(size_t data_length, unsigned char *data);
  void event_list_destroy(event_list_t *list);
  void event_list_add_event(event_list_t *list, size_t data_length, unsigned char *data);
  unsigned char *event_list_get_event(event_list_t *list, size_t index, size_t *data_length);
  size_t event_list_get_event_count(event_list_t *list);
  void event_list_clear(event_list_t *list);

#ifdef __cplusplus
}
#endif

#endif
