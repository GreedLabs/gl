#include <iostream>
#include <boost/variant.hpp>

#include "init.hh"
#include "entity.hh"
#include "camera.hh"
#include "shader/factory.hh"

using namespace std;
using namespace glm;

struct vertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
} __attribute__ ((__packed__));

const char* fragment_shader =
  "#version 330\n"

  "struct SpotLight {"
    "vec3 ambient;"
    "vec3 diffuse;"
    "vec3 specular;"

    "vec3 position;"
    "vec3 direction;"
    "float cutOff;"
    "float outerCutOff;"

    "float constant;"
    "float linear;"
    "float quadratic;"
  "};"

  "struct Material {"
    "sampler2D diffuse;"
    "sampler2D specular;"
    "sampler2D emission;"
    "float shininess;"
  "};"

  "in vec3 Normal;"
  "in vec3 FragPos;"
  "in vec2 Tex;"

  "uniform Material m;"
  "uniform SpotLight s;"
  "uniform vec3 view_position;"

  "out vec4 color;"

  "void main() {"
  "  vec3 ambient  = s.ambient * vec3(texture(m.diffuse, Tex));"
  "  vec3 norm     = normalize(Normal);"
  "  vec3 lightDir = normalize(s.position - FragPos);"
  "  float diff    = max(dot(norm, lightDir), 0.0);"
  "  vec3 diffuse  = s.diffuse * diff * vec3(texture(m.diffuse, Tex));"

  "  float specularStrength = 0.5f;"
  "  vec3 viewDir = normalize(view_position - FragPos);"
  "  vec3 reflectDir = reflect(-lightDir, norm);"
  "  float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);"
  "  vec3 specular = specularStrength * spec *"
  "                  vec3(texture(m.specular , Tex)) * s.specular;"

  " float theta = dot(lightDir, normalize(-s.direction));"
  "  float epsilon = (s.cutOff - s.outerCutOff);"
  "  float intensity = clamp((theta - s.outerCutOff) /"
  "                           epsilon, 0.0, 1.0);"
  "  diffuse  *= intensity;"
  "  specular *= intensity;"

  "  float distance    = length(s.position - FragPos);"
  "  float attenuation = 1.0f / (s.constant + s.linear * distance +"
  "                             s.quadratic * (distance * distance));"
  "  ambient  *= attenuation;"
  "  diffuse  *= attenuation;"
  "  specular *= attenuation;"
  "  color = vec4(ambient + diffuse + specular, 1.0f);"
  "}";

struct vertex vertexData[] = {
  /**********FRONT***********/
  {  0.25f,  0.25f,  0.25f,  0,  0,  1, 1, 1 },
  { -0.25f, -0.25f,  0.25f,  0,  0,  1, 0, 0 },
  {  0.25f, -0.25f,  0.25f,  0,  0,  1, 1, 0 },

  { -0.25f, -0.25f,  0.25f,  0,  0,  1, 0, 0 },
  {  0.25f,  0.25f,  0.25f,  0,  0,  1, 1, 1 },
  { -0.25f,  0.25f,  0.25f,  0,  0,  1, 0, 1 },

  /**********BACK************/
  {  0.25f,  0.25f, -0.25f,  0,  0, -1, 0, 1 },
  {  0.25f, -0.25f, -0.25f,  0,  0, -1, 0, 0 },
  { -0.25f, -0.25f, -0.25f,  0,  0, -1, 1, 0 },

  { -0.25f, -0.25f, -0.25f,  0,  0, -1, 1, 0 },
  { -0.25f,  0.25f, -0.25f,  0,  0, -1, 1, 1 },
  {  0.25f,  0.25f, -0.25f,  0,  0, -1, 0, 1 },

  /**********RIGHT************/
  {  0.25f,  0.25f, -0.25f,  1,  0,  0, 1, 1 },
  {  0.25f, -0.25f,  0.25f,  1,  0,  0, 0, 0 },
  {  0.25f, -0.25f, -0.25f,  1,  0,  0, 1, 0 },

  {  0.25f, -0.25f,  0.25f,  1,  0,  0, 0, 0 },
  {  0.25f,  0.25f, -0.25f,  1,  0,  0, 1, 1 },
  {  0.25f,  0.25f,  0.25f,  1,  0,  0, 0, 1 },

  /**********LEFT***********/
  { -0.25f,  0.25f, -0.25f, -1,  0,  0, 0, 1 },
  { -0.25f, -0.25f, -0.25f, -1,  0,  0, 0, 0 },
  { -0.25f, -0.25f,  0.25f, -1,  0,  0, 1, 0 },

  { -0.25f, -0.25f,  0.25f, -1,  0,  0, 1, 0 },
  { -0.25f,  0.25f,  0.25f, -1,  0,  0, 1, 1 },
  { -0.25f,  0.25f, -0.25f, -1,  0,  0, 0, 1 },

  /**********UP***********/
  {  0.25f,  0.25f, -0.25f,  0,  1,  0, 1, 1 },
  { -0.25f,  0.25f,  0.25f,  0,  1,  0, 0, 0 },
  {  0.25f,  0.25f,  0.25f,  0,  1,  0, 1, 0 },

  { -0.25f,  0.25f,  0.25f,  0,  1,  0, 0, 0 },
  {  0.25f,  0.25f, -0.25f,  0,  1,  0, 1, 1 },
  { -0.25f,  0.25f, -0.25f,  0,  1,  0, 0, 1 },
};

void run(GLFWwindow *w);
void make_resources(vector<EntityPtr<TexturePtr>> &es);
void render(GLFWwindow *w, vector<EntityPtr<TexturePtr>> &es);

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

  vector<EntityPtr<TexturePtr>> es;

  make_resources(es);
  render(w, es);
}

void make_resources(vector<EntityPtr<TexturePtr>> &es) {
  vector<ShaderPtr> v;

  ShaderFactory<TYPELIST_4(SFP::MVP, SFP::Normal,
                           SFP::TextureCoord,
                           SFP::DirectionalLight)>
    factory("", GL_VERTEX_SHADER);
  char *vertex_shader = factory.generate();

  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile(fragment_shader, GL_FRAGMENT_SHADER));

  ProgramPtr p = Program::link(v);

  unsigned vbo = GlObject::mk_buffer(vertexData, sizeof (vertexData));

  TexturePtr td = Texture::fromFile("assets/container.png", GL_LINEAR,
                                    GL_CLAMP_TO_EDGE);
  TexturePtr ts = Texture::fromFile("assets/container_specular.png",
                                    GL_LINEAR, GL_CLAMP_TO_EDGE);
  TexturePtr te = Texture::fromFile("assets/matrix.png",
                                    GL_LINEAR, GL_CLAMP_TO_EDGE);
  for (ssize_t i = 0; i < 15; ++i) {
    EntityPtr<TexturePtr> e = Entity<TexturePtr>::make_entity(p);
    e->bind_buffer(vbo, 10 * 3);
    e->m.diffuse  = td;
    e->m.specular = ts;
    e->m.emission = te;

    e->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);
    e->set_value("normal", 3, GL_FLOAT, sizeof (struct vertex),
        (void *) (offsetof(struct vertex, nx)));
    e->set_value("tex", 2, GL_FLOAT, sizeof (struct vertex),
        (void *) (offsetof(struct vertex, u)));

    e->model = translate(e->model, vec3(-5 + i, 0, -2));
    es.push_back(e);
  }

  free(vertex_shader);
}

void render(GLFWwindow *w, vector<EntityPtr<TexturePtr>> &es) {

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

    vec3 ld = vec3(1, 1, 1) * vec3(0.5f);
    vec3 la = vec3(1, 1, 1) * vec3(0.2f);

    vec3 c = Camera::pos;
    vec3 f = Camera::front;

    EntityPtr<TexturePtr> e = es[0];
    e->p->use();

    glUniform3f(e->p->uniform("m.specular"), 0.633f, 0.7278f, 0.633);
    glUniform1f(e->p->uniform("m.shininess"), 128 * 0.6f);

    glUniform3f(e->p->uniform("s.ambient"), la.x, la.y, la.z);
    glUniform3f(e->p->uniform("s.diffuse"), ld.x, ld.y, ld.z);
    glUniform3f(e->p->uniform("s.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(e->p->uniform("view_position"), c.x, c.y, c.z);

    glUniform3f(e->p->uniform("s.direction"), f.x, f.y, f.z);

    glUniform1f(e->p->uniform("s.constant"), 1.0f);
    glUniform1f(e->p->uniform("s.linear"), 0.09);
    glUniform1f(e->p->uniform("s.quadratic"), 0.032);

    glUniform1f(e->p->uniform("s.cutOff"), cos(radians(12.5f)));
    glUniform1f(e->p->uniform("s.outerCutOff"), cos(radians(17.5f)));

    glUniform3f(e->p->uniform("s.position"), c.x, c.y, c.z);

    for (size_t i = 0; i < es.size(); ++i) {
      e = es[i];
      e->render(time, delta);
    }

    glfwSwapBuffers(w);
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}
