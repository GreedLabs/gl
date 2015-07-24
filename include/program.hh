#ifndef GFX_PROGRAM_HH
#define GFX_PROGRAM_HH

# include <vector>
# include <memory>
# include <epoxy/gl.h>

# include "gl-object.hh"
# include "shader.hh"

struct Program;
using ProgramPtr = boost::intrusive_ptr<Program>;

struct Program: public GlObject {
  Program(unsigned id);

  /// TODO: Use unordered map or some kind of hashmap to mmemoize the data
  int attrib(const char *name) const;
  int uniform(const char *name) const;

  void use();

  static ProgramPtr link(const std::vector<ShaderPtr> &shaders);
};

inline void intrusive_ptr_add_ref(Program *p) {
  ++(p->ref_count);
}

inline void intrusive_ptr_release(Program *p) {
  if(!(--(p->ref_count))) {
    glDeleteProgram(p->id);
    delete p;
  }
}

#endif
