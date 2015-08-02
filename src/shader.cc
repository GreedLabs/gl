#include "shader.hh"

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

  std::ifstream t(file);
  std::string str((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

  return Shader::compile(str.c_str(), shader_type);
}
