#include "entity.hh"

template <>
void Entity<TexturePtr>::render(double time, double delta, mat4 model) {
  (void) delta;
  (void) time;

  glBindVertexArray(id);
  p->use();

  update_mvp(model);

  glActiveTexture(GL_TEXTURE0 + m.diffuse->unit);
  glBindTexture(GL_TEXTURE_2D, m.diffuse->id);
  m.uniform(p, "m.diffuse", m.diffuse);

  glActiveTexture(GL_TEXTURE0 + m.specular->unit);
  glBindTexture(GL_TEXTURE_2D, m.specular->id);
  m.uniform(p, "m.specular", m.specular);

  glActiveTexture(GL_TEXTURE0 + m.emission->unit);
  glBindTexture(GL_TEXTURE_2D, m.emission->id);
  m.uniform(p, "m.emission", m.emission);

  glDrawArrays(GL_TRIANGLES, 0, size);
}
