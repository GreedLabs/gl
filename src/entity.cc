#include "entity.hh"
#include "camera.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

using namespace glm;

mat4 Entity::projection = perspective(
      45.0f,
      (float) (SCREEN_WIDTH) / SCREEN_HEIGHT,
      0.1f, 100.0f
    );

Entity::Entity(unsigned id, ProgramPtr p): GlObject(id), p(p)
{
  glBindVertexArray(id);
  p->use();
}

EntityPtr Entity::make_entity(ProgramPtr p) {
  unsigned id;
  glGenVertexArrays(1, &id);

  EntityPtr e(new Entity(id, p));
  return e;
}


void Entity::bind_buffer(unsigned buff, size_t s,
                 GLenum target) {
  glBindBuffer(target, buff);
  buffer = buff;
  size = s;
}

void Entity::set_value(const char *name,
                       int data_size, GLenum type, size_t stride,
                       void *start) {

  int attrib = p->attrib(name);

  glEnableVertexAttribArray(attrib);
  glVertexAttribPointer(attrib, data_size, type,
                        GL_FALSE, stride, start);
}

void Entity::render(double time, double delta) {
  render(time, delta, model);
}

#include <iostream>
void Entity::render(double time, double delta, mat4 model) {
  (void) delta;
  (void) time;

  glBindVertexArray(id);
  p->use();
  const char *names[] = { "model", "view", "proj" };
  mat4 mats[] = { model, Camera::get_view(), projection };
  for (int i = 0; i < 3; ++i)
    glUniformMatrix4fv(p->uniform(names[i]), 1, 0,
                       value_ptr(mats[i]));

  glDrawArrays(GL_TRIANGLES, 0, size);
}
