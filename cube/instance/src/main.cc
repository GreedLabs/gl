#include <vector>

#include "init.hh"
#include "shader.hh"
#include "program.hh"

using namespace std;

struct vertex {
  float x, y, z;
} __attribute__ ((__packed__));

const char* vertex_shader =
  "#version 330\n"
  "in vec3 vp;"

  "void main () {"
  "  gl_Position = vec4(vp, 1.0);"
  "}";

const char* fragment_shader =
  "#version 330\n"
  "out vec4 color;"

  "void main () {"
  "  color = vec4(0.752, 0.224, 0.168, 1.0);"
  "}";

struct vertex vertexData[] = {
  { -0.25f, -0.25f,  0.25f },
  {  0.25f, -0.25f,  0.25f },
  {  0.25f,  0.25f,  0.25f },
  { -0.25f,  0.25f,  0.25f },

  { -0.25f, -0.25f, -0.25f },
  {  0.25f, -0.25f, -0.25f },
  {  0.25f,  0.25f, -0.25f },
  { -0.25f,  0.25f, -0.25f },
};

unsigned ind[] = {
  2, 0, 1,
  0, 3, 2,

  6, 1, 5,
  5, 6, 2,

  3, 0, 4,
  4, 7, 3,

  0, 3, 4,
  3, 4, 7,

  3, 2, 6,
  3, 6, 7,

  0, 1, 5,
  0, 5, 4
};


unsigned vao, vbo, indices;

void run(GLFWwindow *w);
ProgramPtr make_resources();
void render(GLFWwindow *w, ProgramPtr p);
void cleanup();

int main() {
  GLFWwindow *w = glfw_init();

  run(w);

  glfw_end();
  return 0;
}

void run(GLFWwindow *w) {

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);

  ProgramPtr p = make_resources();
  render(w, p);
  cleanup();
}

ProgramPtr make_resources() {
  vector<ShaderPtr> v;

  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile(fragment_shader, GL_FRAGMENT_SHADER));

  ProgramPtr p = Program::link(v);
  glUseProgram(p->id);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof (vertexData),
               vertexData, GL_STATIC_DRAW);

  glGenBuffers(1, &indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind),
               ind, GL_STATIC_DRAW);

  glEnableVertexAttribArray(glGetAttribLocation(p->id, "vp"));
  glVertexAttribPointer(glGetAttribLocation(p->id, "vp"), 3,
                        GL_FLOAT, GL_FALSE,
                        sizeof (struct vertex), NULL);
  return p;
}

void render(GLFWwindow *w, ProgramPtr p) {
  do {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(p->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

    glDrawElements(GL_TRIANGLES, sizeof (ind) / sizeof (unsigned),
                   GL_UNSIGNED_INT, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glfwSwapBuffers(w);
    glfwPollEvents();
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}

void cleanup() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}
