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

struct Entity;
using EntityPtr = boost::intrusive_ptr<Entity>;

struct Entity: public GlObject {
  Entity(unsigned id, ProgramPtr p);

  void bind_buffer(unsigned buff, size_t s,
                   GLenum target = GL_ARRAY_BUFFER);

  void set_value(const char *name, int data_size,
                 GLenum type, size_t stride, void *start);

  void render(double time, double delta);
  void render(double time, double delta, glm::mat4 model);

  static EntityPtr make_entity(ProgramPtr p);

  ProgramPtr p;
  TexturePtr t;

  glm::mat4 model;

  static glm::mat4 projection;

  unsigned buffer;
  size_t size;

};

inline void intrusive_ptr_add_ref(Entity *p) {
  ++(p->ref_count);
}

inline void intrusive_ptr_release(Entity *p) {
  if(!(--(p->ref_count))) {
    glDeleteVertexArrays(1, &p->id);
    delete p;
  }
}
#endif
