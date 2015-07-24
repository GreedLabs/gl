#include "init.hh"
#include "gl-object.hh"
#include "entity.hh"

using namespace std;
using namespace glm;

struct vertex {
  float x, y, z;
} __attribute__ ((__packed__));

struct directional_light {
  vec3 color;
  float ambiant;
};

const char* vertex_shader =
  "#version 330\n"
  "in vec4 vp;"

  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"

  "void main () {"
  "  gl_Position = proj * view * model * vp;"
  "}";

const char* fragment_shader =
  "#version 330\n"

  "struct directional_light{"
    "vec3 color;"
    "float ambiant;"
  "};"

  "uniform directional_light d;"

  "out vec4 color;"

  "void main() {"
  "  color = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
  "}";

struct vertex vertexData[] = {
  /**********FRONT***********/
  {  0.25f,  0.25f,  0.25f, },
  { -0.25f, -0.25f,  0.25f, },
  {  0.25f, -0.25f,  0.25f, },

  { -0.25f, -0.25f,  0.25f, },
  {  0.25f,  0.25f,  0.25f, },
  { -0.25f,  0.25f,  0.25f, },

  /**********BACK************/
  {  0.25f,  0.25f, -0.25f, },
  {  0.25f, -0.25f, -0.25f, },
  { -0.25f, -0.25f, -0.25f, },

  { -0.25f, -0.25f, -0.25f, },
  { -0.25f,  0.25f, -0.25f, },
  {  0.25f,  0.25f, -0.25f, },

  /**********LEFT************/
  {  0.25f,  0.25f, -0.25f, },
  {  0.25f, -0.25f,  0.25f, },
  {  0.25f, -0.25f, -0.25f, },

  {  0.25f, -0.25f,  0.25f, },
  {  0.25f,  0.25f, -0.25f, },
  {  0.25f,  0.25f,  0.25f, },

  /**********RIGHT***********/
  { -0.25f,  0.25f, -0.25f, },
  { -0.25f, -0.25f, -0.25f, },
  { -0.25f, -0.25f,  0.25f, },

  { -0.25f, -0.25f,  0.25f, },
  { -0.25f,  0.25f,  0.25f, },
  { -0.25f,  0.25f, -0.25f, },
};

void run(GLFWwindow *w);
EntityPtr make_resources();
void render(GLFWwindow *w, EntityPtr e);

int main() {
  GLFWwindow *w = glfw_init();

  run(w);

  glfw_end();
  return 0;
}

unsigned vao, vbo;

void run(GLFWwindow *w) {

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);

  EntityPtr e = make_resources();
  render(w, e);
}

EntityPtr make_resources() {
  vector<ShaderPtr> v;

  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile(fragment_shader, GL_FRAGMENT_SHADER));

  ProgramPtr p = Program::link(v);
  EntityPtr e = Entity::make_entity(p);

  vbo = GlObject::mk_buffer(vertexData, sizeof (vertexData));
  e->bind_buffer(vbo, 8 * 3);
  e->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);

  return e;
}

void render(GLFWwindow *w, EntityPtr e) {

  double prev_frame = glfwGetTime();
  double time = 0;

  e->model = glm::translate(e->model, glm::vec3(0, 0, -2));

  do {

    double curr = glfwGetTime();
    double delta = curr - prev_frame;
    prev_frame = curr;
    time += delta;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 rotated = rotate(e->model,
                          (float) (time * 45.0f * M_PI / 180),
                          glm::vec3(0.0f, 1.0f, 0.0f));
    e->render(time, delta, rotated);

    glfwSwapBuffers(w);
    glfwPollEvents();
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}
