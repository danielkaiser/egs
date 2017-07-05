#include "camera_movement_mixin.hxx"
#include "context.hxx"
#include "gl_context.hxx"

void egs_camera_movement_rotate(egs_camera_movement_ref ctx, float axis_x, float axis_y, float axis_z, float angle) {
  ICameraMovementMixin *camera_movement_mixin = reinterpret_cast<ICameraMovementMixin*>(ctx);
  if (dynamic_cast<Context *>(camera_movement_mixin)) {
    dynamic_cast<Context *>(camera_movement_mixin)->rotate({axis_x, axis_y, axis_z}, angle);
  } else if (dynamic_cast<GLContext *>(camera_movement_mixin)) {
    dynamic_cast<GLContext *>(camera_movement_mixin)->rotate({axis_x, axis_y, axis_z}, angle);
  } else {
    egs_printf(EGS_ERROR, "camera movement called on invalid type\n");
  }
}

void egs_camera_movement_translate(egs_camera_movement_ref ctx, float dir_x, float dir_y, float dir_z) {
  ICameraMovementMixin *camera_movement_mixin = reinterpret_cast<ICameraMovementMixin*>(ctx);
  if (dynamic_cast<Context *>(camera_movement_mixin)) {
    dynamic_cast<Context *>(camera_movement_mixin)->translate({dir_x, dir_y, dir_z});
  } else if (dynamic_cast<GLContext *>(camera_movement_mixin)) {
    dynamic_cast<GLContext *>(camera_movement_mixin)->translate({dir_x, dir_y, dir_z});
  } else {
    egs_printf(EGS_ERROR, "camera movement called on invalid type\n");
  }
}

void egs_camera_movement_zoom(egs_camera_movement_ref ctx, float f) {
  ICameraMovementMixin *camera_movement_mixin = reinterpret_cast<ICameraMovementMixin*>(ctx);
  if (dynamic_cast<Context *>(camera_movement_mixin)) {
    dynamic_cast<Context *>(camera_movement_mixin)->zoom(f);
  } else if (dynamic_cast<GLContext *>(camera_movement_mixin)) {
    dynamic_cast<GLContext *>(camera_movement_mixin)->zoom(f);
  } else {
    egs_printf(EGS_ERROR, "camera movement called on invalid type\n");
  }
}

void egs_camera_movement_set_perspective(egs_camera_movement_ref ctx, float fovy, float aspect, float znear, float zfar) {
  ICameraMovementMixin *camera_movement_mixin = reinterpret_cast<ICameraMovementMixin*>(ctx);
  if (dynamic_cast<Context *>(camera_movement_mixin)) {
    dynamic_cast<Context *>(camera_movement_mixin)->set_perspective(fovy, aspect, znear, zfar);
  } else if (dynamic_cast<GLContext *>(camera_movement_mixin)) {
    dynamic_cast<GLContext *>(camera_movement_mixin)->set_perspective(fovy, aspect, znear, zfar);
  } else {
    egs_printf(EGS_ERROR, "camera movement called on invalid type\n");
  }
}

void egs_camera_movement_look_at(egs_camera_movement_ref ctx,
                                 float camera_position_x, float camera_position_y, float camera_position_z,
                                 float camera_center_x, float camera_center_y, float camera_center_z,
                                 float camera_up_x, float camera_up_y, float camera_up_z) {
  ICameraMovementMixin *camera_movement_mixin = reinterpret_cast<ICameraMovementMixin*>(ctx);
  if (dynamic_cast<Context *>(camera_movement_mixin)) {
    dynamic_cast<Context *>(camera_movement_mixin)->look_at({camera_position_x, camera_position_y, camera_position_z},
                                          {camera_center_x, camera_center_y, camera_center_z},
                                          {camera_up_x, camera_up_y, camera_up_z});
  } else if (dynamic_cast<GLContext *>(camera_movement_mixin)) {
    dynamic_cast<GLContext *>(camera_movement_mixin)->look_at({camera_position_x, camera_position_y, camera_position_z},
                                            {camera_center_x, camera_center_y, camera_center_z},
                                            {camera_up_x, camera_up_y, camera_up_z});
  } else {
    egs_printf(EGS_ERROR, "camera movement called on invalid type\n");
  }
}
