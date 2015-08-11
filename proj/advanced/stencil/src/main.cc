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
void update_uniforms(ProgramPtr p, vec3 l, vec3 la, vec3 ld, vec3 v_pos);

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

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  vector<EntityPtr<TexturePtr>> es;

  make_resources(es);
  render(w, es);
}

EntityPtr<> e2;
ProgramPtr p;

void make_resources(vector<EntityPtr<TexturePtr>> &es) {
  vector<ShaderPtr> v;

  ShaderFactory<TYPELIST_4(SFP::MVP, SFP::Normal,
                           SFP::TextureCoord,
                           SFP::DirectionalLight)>
    factory("", GL_VERTEX_SHADER);
  char *vertex_shader = factory.generate();

  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile_file("shaders/cube.frag",
                                   GL_FRAGMENT_SHADER));

  p = Program::link(v);

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

  v.clear();
  v.push_back(Shader::compile(vertex_shader, GL_VERTEX_SHADER));
  v.push_back(Shader::compile_file("shaders/light.fragment",
                                   GL_FRAGMENT_SHADER));

  p = Program::link(v);
  e2 = Entity<>::make_entity(p);

  e2->bind_buffer(vbo, 8 * 3);
  e2->set_value("vp", 3, GL_FLOAT, sizeof (struct vertex), NULL);

  e2->model = translate(e2->model, vec3(0.0f, 0.0f, 3.0f));
  // e->model = scale(e->model, vec3(0.2f));
  free(vertex_shader);
}

void render(GLFWwindow *w, vector<EntityPtr<TexturePtr>> &es) {

  double prev_frame = glfwGetTime();
  double time = 0;
  double rnd = 0;

  do {

    double curr = glfwGetTime();
    double delta = curr - prev_frame;
    prev_frame = curr;
    time += delta;

    glfwPollEvents();
    Camera::update(delta);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mat4 rotated = e2->model;
    vec3 position = vec3(rotated[3]);

    float angle = (float) (time * 45.0f * M_PI / 180);

    rotated = translate(rotated, -position);
    rotated = rotate(rotated, angle, vec3(0, 1, 0));
    rotated = translate(rotated, position);

    vec3 c = Camera::pos;
    vec3 l = vec3(rotated[3]);
    EntityPtr<TexturePtr> e = es[0];
    vec3 ld = vec3(1, 1, 1) * vec3(0.5f);
    vec3 la = vec3(1, 1, 1) * vec3(0.2f);

    glStencilMask(0x00);
    e2->render(time, delta, rotated);
    update_uniforms(e->p, l, la, ld, c);

    // 1st. Render pass, draw objects as normal, filling the stencil buffer
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF); 
    for (size_t i = 0; i < es.size(); ++i)
      es[i]->render(time, delta);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    for (size_t i = 0; i < es.size(); ++i) {
      ProgramPtr back = es[i]->p;
      es[i]->p = p;

      mat4 scaled = es[i]->model;
      scaled = scale(scaled, vec3(1.01f, 1.01f, 1.01f));

      es[i]->render(time, delta, scaled);
      es[i]->p = back;
    }

    glStencilMask(0xFF);

    if (rnd >= 100) rnd = 0;
    rnd += delta * 10;

    glfwSwapBuffers(w);
  } while (   glfwGetKey(w, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(w) == 0);
}

void update_uniforms(ProgramPtr p, vec3 l, vec3 la, vec3 ld, vec3 v_pos) {
  p->use();
  glUniform3f(p->uniform("v_pos"), v_pos.x, v_pos.y, v_pos.z);

  glUniform3f(p->uniform("m.specular"), 0.633f, 0.7278f, 0.633);
  glUniform1f(p->uniform("m.shininess"), 128 * 0.6f);

  glUniform3f(p->uniform("d.ambient"), 0.1f, 0.1f, 0.1f);
  glUniform3f(p->uniform("d.diffuse"), 0.8f, 0.8f, 0.8f);
  glUniform3f(p->uniform("d.specular"), 1.0f, 1.0f, 1.0f);
  glUniform3f(p->uniform("d.dir"), -0.2f, -1.0f, -0.3f);

  glUniform3f(p->uniform("p_lights[0].pos"), l.x, l.y, l.z);
  glUniform3f(p->uniform("p_lights[0].ambient"), la.x, la.y, la.z);
  glUniform3f(p->uniform("p_lights[0].diffuse"), ld.x, ld.y, ld.z);
  glUniform3f(p->uniform("p_lights[0].specular"), 1.0f, 1.0f, 1.0f);
  glUniform1f(p->uniform("p_lights[0].cst"), 1.0f);
  glUniform1f(p->uniform("p_lights[0].lin"), 0.09);
  glUniform1f(p->uniform("p_lights[0].quad"), 0.032);
}
