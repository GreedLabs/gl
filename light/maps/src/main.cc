#include <iostream>
#include <boost/variant.hpp>

#include "init.hh"
#include "entity.hh"
#include "camera.hh"

using namespace std;
using namespace glm;

struct vertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
} __attribute__ ((__packed__));

const char* vertex_shader =
  "#version 330\n"
  "in vec3 vp;"
  "in vec3 normal;"
  "in vec2 tex;"

  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"

  "out vec3 Normal;"
  "out vec3 FragPos;"
  "out vec2 TexCoord;"

  "void main () {"
  "  Normal = mat3(transpose(inverse(model))) * normal;"
  "  FragPos = vec3(model * vec4(vp, 1.0f));"
  "  gl_Position = proj * view * model * vec4(vp, 1.0);"
  "  TexCoord = tex;"
  "}";

const char* fragment_shader =
  "#version 330\n"

  "struct directional_light {"
    "vec3 ambient;"
    "vec3 diffuse;"
    "vec3 specular;"
    "vec3 pos;"
  "};"

  "struct Material {"
    "sampler2D diffuse;"
    "sampler2D specular;"
    "sampler2D emission;"
    "float shininess;"
  "};"

  "in vec3 Normal;"
  "in vec3 FragPos;"
  "in vec2 TexCoord;"

  "uniform Material m;"
  "uniform directional_light d;"
  "uniform vec3 view_position;"

  "out vec4 color;"

  "float rand(vec2 v) {"
  "  return fract(sin(dot(v.xy ,vec2(12.9898,78.233))) * 43758.5453);"
  "}"

  "void main() {"
  "  vec3 ambient = d.ambient * vec3(texture(m.diffuse, TexCoord));"

  "  vec3 norm     = normalize(Normal);"
  "  vec3 lightDir = normalize(d.pos - FragPos);"
  "  float diff    = max(dot(norm, lightDir), 0.0);"
  "  vec3 diffuse  = d.diffuse * diff * vec3(texture(m.diffuse, TexCoord));"

  "  float specularStrength = 0.5f;"
  "  vec3 viewDir = normalize(view_position - FragPos);"
  "  vec3 reflectDir = reflect(-lightDir, norm);"
  "  float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);"
  "  vec3 specular = specularStrength * spec * vec3(texture(m.specular , TexCoord)) * d.specular;"

  "  vec3 emission = vec3(texture(m.emission, TexCoord - vec2(0, rand(vec2(specular)))));"
  "  vec3 result = ambient + diffuse + specular + emission;"
  "  color = vec4(result, 1.0f);"
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
};

using EntityType = boost::variant<EntityPtr<>, EntityPtr<TexturePtr>>;

void run(GLFWwindow *w);
void make_resources();
void render(GLFWwindow *w);

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

  make_resources();
  render(w);
}

EntityPtr<TexturePtr> e1;
EntityPtr<> e2;

void make_resources() {
  vector<ShaderPtr> v;

  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile(fragment_shader, GL_FRAGMENT_SHADER));

  ProgramPtr p = Program::link(v);

  unsigned vbo = GlObject::mk_buffer(vertexData, sizeof (vertexData));

  e1 = Entity<TexturePtr>::make_entity(p);
  TexturePtr td = Texture::fromFile("assets/container.png", GL_LINEAR,
                                    GL_CLAMP_TO_EDGE);
  TexturePtr ts = Texture::fromFile("assets/container_specular.png",
                                    GL_LINEAR, GL_CLAMP_TO_EDGE);
  TexturePtr te = Texture::fromFile("assets/matrix.png",
                                    GL_LINEAR, GL_CLAMP_TO_EDGE);
  e1->bind_buffer(vbo, 8 * 3);
  e1->m.diffuse  = td;
  e1->m.specular = ts;
  e1->m.emission = te;

  e1->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);
  e1->set_value("normal", 3, GL_FLOAT, sizeof (struct vertex),
               (void *) (offsetof(struct vertex, nx)));
  e1->set_value("tex", 2, GL_FLOAT, sizeof (struct vertex),
               (void *) (offsetof(struct vertex, u)));

  e1->model = translate(e1->model, vec3(0, 0, -1));

  v.clear();
  v.push_back(Shader::compile_file("shaders/light.vertex",
                                   GL_VERTEX_SHADER));
  v.push_back(Shader::compile_file("shaders/light.fragment",
                                   GL_FRAGMENT_SHADER));

  p = Program::link(v);
  e2 = Entity<>::make_entity(p);

  e2->bind_buffer(vbo, 8 * 3);
  e2->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);

  e2->model = translate(e2->model, vec3(0.0f, 0.0f, 2.0f));
  // e->model = scale(e->model, vec3(0.2f));
}

void render(GLFWwindow *w) {

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
    e1->p->use();

    glUniform3f(e1->p->uniform("m.specular"), 0.633f, 0.727811f, 0.633);
    glUniform1f(e1->p->uniform("m.shininess"), 128 * 0.6f);

    glUniform3f(e1->p->uniform("d.ambient"), la.x, la.y, la.z);
    glUniform3f(e1->p->uniform("d.diffuse"), ld.x, ld.y, ld.z);
    glUniform3f(e1->p->uniform("d.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(e1->p->uniform("view_position"), c.x, c.y, c.z);


    mat4 rotated = e2->model;

    float angle = (float) (time * 45.0f * M_PI / 180);
    vec3 position = vec3(rotated[3]);

    rotated = translate(rotated, -position);
    rotated = rotate(rotated, angle, vec3(0, 1, 0));
    rotated = translate(rotated, position);

    vec3 l = vec3(rotated[3]);
    glUniform3f(e1->p->uniform("d.pos"), l.x, l.y, l.z);

    e2->render(time, delta, rotated);
    e1->render(time, delta);

    glfwSwapBuffers(w);
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}
