#include "entity.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

Entity::Entity(unsigned id): GlObject(id)
{
  projection = glm::perspective(45.0f, (float) (SCREEN_WIDTH) / SCREEN_HEIGHT,
                                0.1f, 100.0f);
  view = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

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
  (void) delta;
  (void) time;

  glm::mat4 rotated  = rotate(model, (float) (((time * 45.0f) * M_PI) / 180),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  glUseProgram(p->id);
  const char *names[] = { "model", "view", "proj" };
  glm::mat4 mats[] = { rotated, view, projection };
  for (int i = 0; i < 3; ++i)
    glUniformMatrix4fv(p->uniform(names[i]), 1, 0,
                       glm::value_ptr(mats[i]));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
  glDrawArrays(GL_TRIANGLES, 0, buffers_sizes[0] / 3);
}
