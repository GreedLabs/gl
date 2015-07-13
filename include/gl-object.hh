#ifndef GL_OBJECT_HH
# define GL_OBJECT_HH

# include <err.h>
# include <epoxy/gl.h>
# include <GLFW/glfw3.h>
# include <boost/intrusive_ptr.hpp>

typedef void (*getIv) (unsigned, GLenum, int *);
typedef void (*getLog) (unsigned, int, int *, char *);

struct GlObject;
using GlObjectPtr = boost::intrusive_ptr<GlObject>;

struct GlObject {
  GlObject(unsigned id): id(id), ref_count(0) {}

  static void check_errors(unsigned id, getIv iv, getLog log,
                           int gl_status, const char *msg) {
    int status = 0;
    iv(id, gl_status, &status);

    if (status == GL_FALSE) {
      int info_log_len = 0;

      iv(id, GL_INFO_LOG_LENGTH, &info_log_len);
      char *str_info_log = new char[info_log_len + 1];
      log(id, info_log_len, NULL, str_info_log);

      errx(1, "%s\n%s\n", msg, str_info_log);
    }
  }

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
