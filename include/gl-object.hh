#ifndef GL_OBJECT_HH
# define GL_OBJECT_HH

# include <err.h>
# include <epoxy/gl.h>
# include <GLFW/glfw3.h>
# include <boost/intrusive_ptr.hpp>
# include <vector>

typedef void (*getIv) (unsigned, GLenum, int *);
typedef void (*getLog) (unsigned, int, int *, char *);

struct GlObject;
using GlObjectPtr = boost::intrusive_ptr<GlObject>;

struct GlObject {
  GlObject(unsigned id);

  static void check_errors(unsigned id, getIv iv, getLog log,
                           int gl_status, const char *msg);

  static unsigned mk_buffer(void *buffer, size_t size,
                     GLenum target = GL_ARRAY_BUFFER,
                     GLenum usage = GL_STATIC_DRAW);
  static void clean_buffers();
  static std::vector<unsigned> buffers;

  unsigned id;
  unsigned ref_count;
};


inline void intrusive_ptr_add_ref(GlObject *o) {
  ++(o->ref_count);
}

inline void intrusive_ptr_release(GlObject *o) {
  if(!(--(o->ref_count)))
   delete o;
}

#endif
