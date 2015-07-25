#include <iostream>

#include "init.hh"
#include "entity.hh"
#include "camera.hh"

using namespace std;
using namespace glm;

struct vertex {
  float x, y, z;
  float nx, ny, nz;
} __attribute__ ((__packed__));

struct directional_light {
  vec3 color;
  float ambiant;
};

const char* vertex_shader =
  "#version 330\n"
  "in vec3 vp;"
  "in vec3 normal;"

  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"

  "out vec3 Normal;"
  "out vec3 FragPos;"

  "void main () {"
  "  Normal = mat3(transpose(inverse(model))) * normal;"
  "  FragPos = vec3(model * vec4(vp, 1.0f));"
  "  gl_Position = proj * view * model * vec4(vp, 1.0);"
  "}";

const char* fragment_shader =
  "#version 330\n"

  "struct directional_light{"
    "vec3 color;"
    "vec3 pos;"
  "};"

  "in vec3 Normal;"
  "in vec3 FragPos;"

  "uniform directional_light d;"
  "uniform vec3 obj_color;"

  "out vec4 color;"

  "void main() {"
  "  float ambientStrength = 0.5f;"
  "  vec3 ambient = ambientStrength * d.color;"

  "  vec3 norm     = normalize(Normal);"
  "  vec3 lightDir = normalize(d.pos - FragPos);"
  "  float diff    = max(dot(norm, lightDir), 0.0);"
  "  vec3 diffuse  = diff * d.color;"

  "  vec3 result = (ambient + diffuse) * obj_color;"
  "  color = vec4(result, 1.0f);"
  "}";

struct vertex vertexData[] = {
  /**********FRONT***********/
  {  0.25f,  0.25f,  0.25f,  0,  0,  1 },
  { -0.25f, -0.25f,  0.25f,  0,  0,  1 },
  {  0.25f, -0.25f,  0.25f,  0,  0,  1 },

  { -0.25f, -0.25f,  0.25f,  0,  0,  1 },
  {  0.25f,  0.25f,  0.25f,  0,  0,  1 },
  { -0.25f,  0.25f,  0.25f,  0,  0,  1 },

  /**********BACK************/
  {  0.25f,  0.25f, -0.25f,  0,  0, -1 },
  {  0.25f, -0.25f, -0.25f,  0,  0, -1 },
  { -0.25f, -0.25f, -0.25f,  0,  0, -1 },

  { -0.25f, -0.25f, -0.25f,  0,  0, -1 },
  { -0.25f,  0.25f, -0.25f,  0,  0, -1 },
  {  0.25f,  0.25f, -0.25f,  0,  0, -1 },

  /**********RIGHT************/
  {  0.25f,  0.25f, -0.25f,  1,  0,  0 },
  {  0.25f, -0.25f,  0.25f,  1,  0,  0 },
  {  0.25f, -0.25f, -0.25f,  1,  0,  0 },

  {  0.25f, -0.25f,  0.25f,  1,  0,  0 },
  {  0.25f,  0.25f, -0.25f,  1,  0,  0 },
  {  0.25f,  0.25f,  0.25f,  1,  0,  0 },

  /**********LEFT***********/
  { -0.25f,  0.25f, -0.25f, -1,  0,  0 },
  { -0.25f, -0.25f, -0.25f, -1,  0,  0 },
  { -0.25f, -0.25f,  0.25f, -1,  0,  0 },

  { -0.25f, -0.25f,  0.25f, -1,  0,  0 },
  { -0.25f,  0.25f,  0.25f, -1,  0,  0 },
  { -0.25f,  0.25f, -0.25f, -1,  0,  0 },
};

void run(GLFWwindow *w);
void make_resources(vector<EntityPtr> &es);
void render(GLFWwindow *w, vector<EntityPtr> &es);

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

  vector<EntityPtr> es;
  make_resources(es);
  render(w, es);
}

unsigned cvao, lvao;
unsigned vbo;
ProgramPtr p1, p2;

void make_resources(vector<EntityPtr> &es) {
  vector<ShaderPtr> v;

  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile(fragment_shader, GL_FRAGMENT_SHADER));

  ProgramPtr p = Program::link(v);

  unsigned vbo = GlObject::mk_buffer(vertexData, sizeof (vertexData));

  EntityPtr  e = Entity::make_entity(p);
  e->bind_buffer(vbo, 8 * 3);
  
  e->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);
  e->set_value("normal", 3, GL_FLOAT, sizeof (struct vertex),
               (void *) (3 * sizeof (float)));

  e->model = glm::translate(e->model, glm::vec3(0, 0, -2));

  es.push_back(e);
  v.clear();

  v.push_back(Shader::compile_file("shaders/light.vertex",
                                   GL_VERTEX_SHADER));
  v.push_back(Shader::compile_file("shaders/light.fragment",
                                   GL_FRAGMENT_SHADER));

  p = Program::link(v);
  e = Entity::make_entity(p);

  e->bind_buffer(vbo, 8 * 3);
  e->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);

  e->model = glm::translate(e->model, glm::vec3(-1.0f, 0.0f, -2.0f));

  es.push_back(e);

}

void render(GLFWwindow *w, vector<EntityPtr> &es) {

  double prev_frame = glfwGetTime();
  double time = 0;

  do {

    double curr = glfwGetTime();
    double delta = curr - prev_frame;
    prev_frame = curr;
    time += delta;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Camera::update(delta);

    vec3 light_pos = vec3(es[1]->model[3]);
    es[0]->p->use();
    glUniform3f(es[0]->p->uniform("obj_color"),
                0.021f, 0.174f, 0.021f);
    glUniform3f(es[0]->p->uniform("d.pos"),
                light_pos.x, light_pos.y, light_pos.z);
    glUniform3f(es[0]->p->uniform("d.color"), 1.0f, 1.0f, 0.0f);

    float angle = (float) (time * 45.0f * M_PI / 180);
    for (size_t i = 0; i < es.size(); ++i) {
      mat4 rotated = rotate(es[i]->model, angle, vec3(0.0f, 1.0f, 0.0f));
      es[i]->render(time, delta, rotated);
    }

    glfwSwapBuffers(w);
    glfwPollEvents();
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}
