#include "glip_event_list.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

event_list_t *event_list_create() {
  event_list_t *list = (event_list_t *)malloc(sizeof(event_list_t));
  list->data = NULL;
  event_list_clear(list);
  return list;
}

event_list_t *event_list_create_from_char_ptr(size_t data_length, unsigned char *data) {
  assert(data_length >= sizeof(size_t));
  event_list_t *list = (event_list_t *)malloc(sizeof(event_list_t));
  list->data = malloc(data_length);
  assert(list->data);
  memcpy(list->data, data, data_length);
  list->current_length = data_length;
  return list;
}

void event_list_destroy(event_list_t *list) {
  free(list->data);
  free(list);
}

void event_list_add_event(event_list_t *list, size_t data_length, unsigned char *data) {
  size_t new_length = list->current_length + data_length + sizeof(size_t);
  list->data = realloc(list->data, new_length);
  assert(list->data);
  *((size_t *)list->data) = *((size_t *)list->data) + 1;
  *((size_t *)(list->data + list->current_length)) = data_length;
  memcpy(list->data + list->current_length + sizeof(size_t), data, data_length);
  list->current_length = new_length;
}

unsigned char *event_list_get_event(event_list_t *list, size_t index, size_t *data_length) {
  size_t i = 0;
  size_t offset = sizeof(size_t);
  if (index >= event_list_get_event_count(list)) {
    return NULL;
  }
  while (i < index) {
    offset += (*((size_t *)(list->data+offset))) + sizeof(size_t);
    i++;
  }
  *data_length = *((size_t *)(list->data+offset));
  return list->data + offset + sizeof(size_t);
}

size_t event_list_get_event_count(event_list_t *list) {
  return *((size_t *)list->data);
}

void event_list_clear(event_list_t *list) {
  list->current_length = sizeof(size_t);
  list->data = realloc(list->data, list->current_length);
  *((size_t *)list->data) = 0;
}
