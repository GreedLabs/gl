#ifndef ENTITY_HH
# define ENTITY_HH

# include <memory>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <glm/ext.hpp>

# include "gl-object.hh"
# include "texture.hh"
# include "program.hh"

#include "material.hh"

template <typename Mat = glm::vec3>
struct Entity;

template <typename T = glm::vec3>
using EntityPtr = boost::intrusive_ptr<Entity<T>>;

template <typename Mat>
struct Entity: public GlObject {
  Entity(unsigned id, ProgramPtr p);

  void bind_buffer(unsigned buff, size_t s,
                   GLenum target = GL_ARRAY_BUFFER);

  void set_value(const char *name, int data_size,
                 GLenum type, size_t stride, void *start);

  void render(double time, double delta);
  void render(double time, double delta, glm::mat4 model);

  static EntityPtr<Mat> make_entity(ProgramPtr p);

  ProgramPtr p;
  TexturePtr t;

  glm::mat4 model;

  static glm::mat4 projection;

  unsigned buffer;
  size_t size;
  Material<Mat> m;

  // assume program was used
  void update_mvp(glm::mat4 model);
};

#include "entity.hxx"

template <typename T>
inline void intrusive_ptr_add_ref(Entity<T> *p) {
  ++(p->ref_count);
}

template <typename T>
inline void intrusive_ptr_release(Entity<T> *p) {
  if(!(--(p->ref_count))) {
    glDeleteVertexArrays(1, &p->id);
    delete p;
  }
}
#endif
