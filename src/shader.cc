#include "shader.hh"

#include <string>
#include <epoxy/gl.h>

Shader::Shader(unsigned id): GlObject(id) {}

ShaderPtr Shader::compile(const char *shader_code, unsigned shader_type)
{
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
