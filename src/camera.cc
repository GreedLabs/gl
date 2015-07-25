#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "camera.hh"
#include "entity.hh"

using namespace glm;

#define VELOCITY 2
#define SENSITIVITY 0.10f

bool Camera::Camera::keys[CAMERA_HH_KEYS];
double Camera::yaw = -90, Camera::pitch = 0;
vec3 Camera::pos     = vec3(0, 0, 0),
     Camera::front   = vec3(0, 0, -1),
     Camera::up      = vec3(0, 1, 0),
     Camera::right   = vec3(1, 0, 0),
     Camera::worldup = up;

void Camera::key_callback(GLFWwindow* window, int key,
                          int scancode, int action, int mode) {
  (void) scancode;
  (void) action;
  (void) mode;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      Camera::keys[key] = true;
    else if (action == GLFW_RELEASE)
      Camera::keys[key] = false;
  }

}

void Camera::update(double delta) {
  if (Camera::keys[GLFW_KEY_W])
    Camera::process_translate(FORWARD, delta);
  if (Camera::keys[GLFW_KEY_S])
    Camera::process_translate(BACKWARD, delta);
  if (Camera::keys[GLFW_KEY_A])
    Camera::process_translate(LEFT, delta);
  if (Camera::keys[GLFW_KEY_D])
    Camera::process_translate(RIGHT, delta);
}

void Camera::mouse_callback(GLFWwindow* window,
                            double xpos, double ypos) {

  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  if (state != GLFW_PRESS)
    return;

  static bool firstMouse = true;
  static double lastX, lastY;
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  GLfloat xoffset = xpos - lastX;
  GLfloat yoffset = lastY - ypos;  // y-coordinates: bottom to left

  lastX = xpos;
  lastY = ypos;

  process_rotation(xoffset, yoffset);
}

void Camera::Camera::process_translate(Direction d, double delta) {
  float velocity = VELOCITY * delta;

  if (d == FORWARD)
    pos += front * velocity;
  else if (d == BACKWARD)
    pos -= front * velocity;
  else if (d == LEFT)
    pos -= right * velocity;
  else if (d == RIGHT)
    pos += right * velocity;
  else
    return;

  update_view();
}

void Camera::process_rotation (double x, double y) {
  x *= SENSITIVITY;
  y *= SENSITIVITY;

  yaw   += x;
  pitch += y;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;


  update_view();
}

void Camera::update_view() {
  vec3 f;
  f.x = cos(radians(yaw)) * cos(radians(pitch));
  f.y = sin(radians(pitch));
  f.z = sin(radians(yaw)) * cos(radians(pitch));
  front = normalize(f);

  right = normalize(cross(front, worldup));
  up    = normalize(cross(right, front));
}

mat4 Camera::get_view() {
  return glm::lookAt(pos, pos + front, up);
}
