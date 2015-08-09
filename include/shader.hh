#ifndef SHADER_HH
#define SHADER_HH

# include <memory>
# include <epoxy/gl.h>

# include "gl-object.hh"

struct Shader;
using ShaderPtr = boost::intrusive_ptr<Shader>;

struct Shader: GlObject {
  Shader(unsigned id);

  static ShaderPtr compile(const char *shader_code, unsigned shader_type);

  // If path starts with './' it won't be relative
  // to executable path.
  static ShaderPtr compile_file(const char *file, unsigned shader_type);
};

inline void intrusive_ptr_add_ref(Shader *p) {
  ++(p->ref_count);
}

inline void intrusive_ptr_release(Shader *p) {
  if(!(--(p->ref_count))) {
    glDeleteShader(p->id);
    delete p;
  }
}

#endif
