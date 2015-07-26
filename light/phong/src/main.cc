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
  "uniform vec3 view_position;"

  "out vec4 color;"

  "void main() {"
  "  float ambientStrength = 0.1f;"
  "  vec3 ambient = ambientStrength * d.color;"

  "  vec3 norm     = normalize(Normal);"
  "  vec3 lightDir = normalize(d.pos - FragPos);"
  "  float diff    = max(dot(norm, lightDir), 0.0);"
  "  vec3 diffuse  = diff * d.color;"

  "  float specularStrength = 0.5f;"
  "  vec3 viewDir = normalize(view_position - FragPos);"
  "  vec3 reflectDir = reflect(-lightDir, norm);"
  "  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
  "  vec3 specular = specularStrength * spec * d.color;"

  "  vec3 result = (ambient + diffuse + specular) * obj_color;"
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

  e->model = translate(e->model, vec3(0, 0, -1));

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

  e->model = translate(e->model, vec3(0.0f, 0.0f, 1.0f));
  e->model = scale(e->model, vec3(0.2f));

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

    glfwPollEvents();
    Camera::update(delta);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    vec3 light_pos = vec3(es[1]->model[3]);
    es[0]->p->use();
    glUniform3f(es[0]->p->uniform("obj_color"), 1.0f, 0.5f, 0.31f);
    glUniform3f(es[0]->p->uniform("d.color"), 1.0f, 1.0f, 1.0f);
    glUniform3f(es[0]->p->uniform("d.pos"),
                light_pos.x, light_pos.y, light_pos.z);
    glUniform3f(es[0]->p->uniform("view_position"),
                Camera::pos.x, Camera::pos.y, Camera::pos.z);

    for (size_t i = 0; i < es.size(); ++i) {
      mat4 rotated = es[i]->model;
      es[i]->render(time, delta, rotated);
    }

    glfwSwapBuffers(w);
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}
