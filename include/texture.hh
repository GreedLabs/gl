#ifndef TEXTURE_HH
# define TEXTURE_HH

#include <memory>
#include "gl-object.hh"

struct Texture;
using TexturePtr = boost::intrusive_ptr<Texture>;

struct Texture: public GlObject {
  Texture(unsigned id);
  static TexturePtr fromFile(const char *filename,
                             int min_mag_filter, int wrap_mode);
};

inline void intrusive_ptr_add_ref(Texture *p) {
  ++(p->ref_count);
}

inline void intrusive_ptr_release(Texture *p) {
  if(!(--(p->ref_count))) {
    glDeleteProgram(p->id);
    delete p;
  }
}

#endif
