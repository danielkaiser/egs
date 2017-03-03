#ifndef CAMERA_MOVEMENT_MIXIN_H
#define CAMERA_MOVEMENT_MIXIN_H

#include "propertystore.hxx"
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

template <typename T>
class CameraMovementMixin {
public:
  CameraMovementMixin(T &ctx) : ctx(ctx) {}

  void rotate(glm::vec3 axis, float angle) {
    auto camera_position = get_property<glm::vec3>("camera_position");
    auto camera_center = get_property<glm::vec3>("camera_center");
    auto camera_up = get_property<glm::vec3>("camera_up");
    auto camera_forward = camera_center-camera_position;
    auto camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
    axis = axis.x * camera_right + axis.y * camera_up + axis.z * camera_forward;
    camera_position = glm::vec3(glm::rotate(angle, axis)*glm::vec4(camera_position, 1));
    camera_up = glm::normalize(glm::cross(camera_right, camera_center-camera_position));
    set_property("camera_position", camera_position);
    set_property("camera_center", camera_center);
    set_property("camera_up", camera_up);
    set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
  }

  void translate(glm::vec3 dir) {
    auto camera_position = get_property<glm::vec3>("camera_position");
    auto camera_center = get_property<glm::vec3>("camera_center");
    auto camera_up = get_property<glm::vec3>("camera_up");
    auto camera_forward = camera_center-camera_position;
    auto camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
    dir = dir.x * camera_right + dir.y * camera_up + dir.z * camera_forward;
    camera_center += dir;
    camera_position += dir;
    set_property("camera_position", camera_position);
    set_property("camera_center", camera_center);
    auto view_matrix = glm::lookAt(camera_position, camera_center, camera_up);
    set_property("view_matrix", view_matrix);
  }

  void zoom(float f) {
    auto camera_position = get_property<glm::vec3>("camera_position");
    auto camera_center = get_property<glm::vec3>("camera_center");
    auto camera_up = get_property<glm::vec3>("camera_up");
    camera_position = (camera_position - camera_center)*f + camera_center;
    set_property("camera_position", camera_position);
    set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
  }

  void set_perspective(float fovy, float aspect, float znear=0.1, float zfar=1000) {
    set_property<glm::mat4>("projection", glm::perspective(fovy, aspect, znear, zfar));
  }

  void look_at(glm::vec3 camera_position, glm::vec3 camera_center, glm::vec3 camera_up) {
    set_property("camera_position", camera_position);
    set_property("camera_center", camera_center);
    set_property("camera_up", camera_up);
    set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
  }

private:
  template<typename property_type>
  void set_property(const std::string& property_name, const property_type& value) {
    ctx.template set_property<property_type>(property_name, value);
  }

  template<typename property_type>
  property_type& get_property(const std::string& property_name, const property_type& default_value=property_type()) {
    return ctx.template get_property<property_type>(property_name, default_value);
  }

  T &ctx;
};

#endif
