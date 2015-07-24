#include <err.h>

#include "init.hh"
#include "gl-object.hh"
#include "camera.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

struct GLFW_config {
  unsigned name, value;
};

GLFWwindow *glfw_init(unsigned *hints, size_t n) {

  if (!glfwInit())
    errx(1, "Failed to init GLFW");

  struct GLFW_config *configs = (struct GLFW_config *) hints;
  for (size_t i = 0; i < n; ++i)
    glfwWindowHint(configs[i].name, configs[i].value);

  GLFWwindow *window = NULL;

  if (!(window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                                  "Triangle", NULL, NULL)))
    errx(1, "Failed to open GLFW window.\n");

  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, Camera::key_callback);
  glfwSetCursorPosCallback(window, Camera::mouse_callback);

  return window;
}

GLFWwindow *glfw_init() {
  unsigned hints[] = {
    GLFW_SAMPLES, 4,
    GLFW_CONTEXT_VERSION_MAJOR, 3,
    GLFW_CONTEXT_VERSION_MINOR, 3,
    GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE
  };

  return glfw_init(hints, 4);
}

void glfw_end() {
  GlObject::clean_buffers();
  glfwTerminate();
}
