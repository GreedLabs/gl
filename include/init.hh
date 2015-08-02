#ifndef INIT_HH
# define INIT_HH

# include <epoxy/gl.h>
# include <GLFW/glfw3.h>

# define SCREEN_WIDTH  800
# define SCREEN_HEIGHT 600

/*
 *  Will init glfw with the parameters in config array
 *  as well as create a new window and init the context
 *  @parameter hints: an array with:
 *    n % 2 == 0: The config hint
 *    n % 2 == 1: The config value
 *  @parameter n: number of hints
 */
GLFWwindow *glfw_init(unsigned *hints, size_t n);

/*
 * Will call glfw_init with the following values:
 *  GLFW_SAMPLES 4
 *  GLFW_CONTEXT_VERSION_MAJOR 3
 *  GLFW_CONTEXT_VERSION_MINOR 3
 *  GLFW_RESIZABLE 0
 *  GLFW_OPENGL_PROFILE GLFW_OPENGL_CORE_PROFILE
 *
 */
GLFWwindow *glfw_init();

/*
 *  Will init gl with the parameters in hints array
 *  @parameter hints: hints to send to opengl
 *  @parameter n: number of hints
 */
void glfw_end();

#endif
