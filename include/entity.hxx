#include "entity.hh"
#include "camera.hh"

#define SCREEN_WIDTH_HXX 1024
#define SCREEN_HEIGHT_HXX 768

using namespace glm;

template <typename T>
mat4 Entity<T>::projection = perspective(
      45.0f,
      (float) (SCREEN_WIDTH_HXX) / SCREEN_HEIGHT_HXX,
      0.1f, 100.0f
    );

template <typename T>
Entity<T>::Entity(unsigned id, ProgramPtr p): GlObject(id), p(p)
{
  glBindVertexArray(id);
  p->use();
}

template <typename T>
EntityPtr<T> Entity<T>::make_entity(ProgramPtr p) {
  unsigned id;
  glGenVertexArrays(1, &id);

  EntityPtr<T> e(new Entity<T>(id, p));
  return e;
}

template <typename T>
void Entity<T>::bind_buffer(unsigned buff, size_t s,
                 GLenum target) {
  glBindBuffer(target, buff);
  buffer = buff;
  size = s;
}

template <typename T>
void Entity<T>::set_value(const char *name,
                       int data_size, GLenum type, size_t stride,
                       void *start) {

  int attrib = p->attrib(name);

  glEnableVertexAttribArray(attrib);
  glVertexAttribPointer(attrib, data_size, type,
                        GL_FALSE, stride, start);
}

template <typename T>
void Entity<T>::render(double time, double delta) {
  render(time, delta, model);
}

template <typename T>
void Entity<T>::render(double time, double delta, mat4 model) {
  (void) delta;
  (void) time;

  glBindVertexArray(id);
  p->use();

  m.uniform(p, "m.diffuse", m.diffuse);
  m.uniform(p, "m.specular", m.specular);

  const char *names[] = { "model", "view", "proj" };
  mat4 mats[] = { model, Camera::get_view(), projection };
  for (int i = 0; i < 3; ++i)
    glUniformMatrix4fv(p->uniform(names[i]), 1, 0,
                       value_ptr(mats[i]));

  glDrawArrays(GL_TRIANGLES, 0, size);
}

template <>
void Entity<TexturePtr>::render(double time, double delta, mat4 model) {
  (void) delta;
  (void) time;

  glBindVertexArray(id);
  p->use();
  const char *names[] = { "model", "view", "proj" };
  mat4 mats[] = { model, Camera::get_view(), projection };
  for (int i = 0; i < 3; ++i)
    glUniformMatrix4fv(p->uniform(names[i]), 1, 0,
                       value_ptr(mats[i]));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m.diffuse->id);
  m.uniform(p, "m.diffuse", m.diffuse);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m.specular->id);
  m.uniform(p, "m.specular", m.specular);

  glDrawArrays(GL_TRIANGLES, 0, size);
}
