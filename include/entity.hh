#ifndef ENTITY_HH
# define ENTITY_HH

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "gl-object.hh"
#include "texture.hh"
#include "program.hh"

struct Entity;
using EntityPtr = boost::intrusive_ptr<Entity>;

struct Entity: public GlObject {
  Entity(unsigned id);

  unsigned mk_buffer(void *buffer, size_t size,
                     GLenum target = GL_ARRAY_BUFFER,
                     GLenum usage = GL_STATIC_DRAW);
  void set_value(const char *name, int data_size,
                 GLenum type, size_t stride, void *start);

  void update(double time, double delta);

  static EntityPtr make_entity(ProgramPtr p);

  ProgramPtr p;
  TexturePtr t;

  glm::mat4 model, view, projection;
  std::vector<unsigned> buffers;
  std::vector<size_t> buffers_sizes;
};

inline void intrusive_ptr_add_ref(Entity *p) {
  ++(p->ref_count);
}

inline void intrusive_ptr_release(Entity *p) {
  if(!(--(p->ref_count))) {
    for (size_t i = 0; i < p->buffers.size(); ++i)
      glDeleteBuffers(1, &p->buffers[i]);
    glDeleteVertexArrays(1, &p->id);
    delete p;
  }
}
#endif
