#ifndef CAMERA_HH
# define CAMERA_HH

# define CAMERA_HH_KEYS 1024

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <glm/ext.hpp>

struct Camera {
  enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
  };

  static void key_callback(GLFWwindow* window, int key,
                           int scancode, int action, int mode);
  static void mouse_callback(GLFWwindow* window,
                             double xpos, double ypos);
  static void update(double delta);

  static void process_translate(Direction d, double delta);
  static void process_rotation (double x, double y);

  static void update_view();

  static glm::mat4 get_view();

  static bool keys[CAMERA_HH_KEYS];

  static double yaw, pitch;
  static glm::vec3 pos, front, up, right, worldup;
};

#endif
