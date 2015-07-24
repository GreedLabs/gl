#include <algorithm>
#include "gl-object.hh"

std::vector<unsigned> GlObject::buffers;

GlObject::GlObject(unsigned id): id(id), ref_count(0)
{}

void GlObject::check_errors(unsigned id,
                            getIv iv, getLog log,
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

unsigned GlObject::mk_buffer(void *buffer, size_t size,
                             GLenum target, GLenum usage) {
  unsigned vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(target, vbo);
  glBufferData(target, size, buffer, usage);

  return vbo;
}

void GlObject::clean_buffers() {
  std::for_each(buffers.begin(), buffers.end(),
      [](auto &it) {
          glDeleteBuffers(1, &it);
      });
}
