#include "entity.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

using namespace glm;

mat4 Entity::projection = perspective(
      45.0f,
      (float) (SCREEN_WIDTH) / SCREEN_HEIGHT,
      0.1f, 100.0f
    );
mat4 Entity::view = lookAt(
      vec3(0, 0, 0),
      vec3(0, 0, -1),
      vec3(0, 1, 0)
    );

Entity::Entity(unsigned id): GlObject(id)
{}

EntityPtr Entity::make_entity(ProgramPtr p) {
  unsigned id;
  glGenVertexArrays(1, &id);

  EntityPtr e(new Entity(id));
  e->p = p;
  return e;
}

unsigned Entity::mk_buffer(void *buffer, size_t size,
                           GLenum target, GLenum usage) {

  glBindVertexArray(id);
  glUseProgram(p->id);

  unsigned vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(target, vbo);
  glBufferData(target, size, buffer, usage);

  buffers.push_back(vbo);
  buffers_sizes.push_back(size);
  return vbo;
}

void Entity::set_value(const char *name,
                       int data_size, GLenum type, size_t stride,
                       void *start) {


  int attrib = p->attrib(name);

  glEnableVertexAttribArray(attrib);
  glVertexAttribPointer(attrib, data_size, type,
                        GL_FALSE, stride, start);
}
void Entity::update(double time, double delta) {
  update(time, delta, model);
}

void Entity::update(double time, double delta, mat4 model) {
  (void) delta;
  (void) time;

  glUseProgram(p->id);
  const char *names[] = { "model", "view", "proj" };
  mat4 mats[] = { model, view, projection };
  for (int i = 0; i < 3; ++i)
    glUniformMatrix4fv(p->uniform(names[i]), 1, 0,
                       value_ptr(mats[i]));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
  glDrawArrays(GL_TRIANGLES, 0, buffers_sizes[0] / 3);
}
