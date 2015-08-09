#include "shader.hh"

#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <epoxy/gl.h>

Shader::Shader(unsigned id): GlObject(id) {}

ShaderPtr Shader::compile(const char *shader_code, unsigned shader_type) {
  unsigned id = glCreateShader(shader_type);
  if (!id)
    errx(1, "Unable to create a shader");

  ShaderPtr shader(new Shader(id));
  glShaderSource(id, 1, &shader_code, NULL);
  glCompileShader(id);

  check_errors(id, glGetShaderiv, glGetShaderInfoLog,
               GL_COMPILE_STATUS, "Shader Compiling");

  return shader;
}

ShaderPtr Shader::compile_file(const char *file, unsigned shader_type) {

  static bool path_initialized = false;
  static char abs_path[1024];
  static char *abs_dir;

  char *f = (char *) file;

  if (!path_initialized) {
    path_initialized = true;
    #if defined(__linux)
      const char *path = "/proc/self/exe";
    #else // then freebsd
      const char *path = "/proc/curproc/file";
    #endif
    ssize_t s = readlink(path, abs_path, 1024);
    abs_path[s] = 0;
    abs_dir = dirname(abs_path);
  }

  if (!(file[0] == '.' && file[0] != '\0' && file[1] == '/')) {
    size_t s = strlen(abs_dir);

    f = (char *) calloc(1, s + strlen(file) + 2);

    strcat(f, abs_dir);
    strcat(f + s + 1, file);

    f[s] = '/';
  }

  std::ifstream t(f);
  std::string str((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

  return Shader::compile(str.c_str(), shader_type);
}
