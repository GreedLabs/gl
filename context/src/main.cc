#include "init.hh"
#include <unistd.h>

int main() {
  glfw_init();
  glfw_end();

  return 0;
}
