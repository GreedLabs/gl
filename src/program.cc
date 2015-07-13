#include "program.hh"
#include "shader.hh"

#include <string>
#include <epoxy/gl.h>

Program::Program(unsigned id): GlObject(id)
{}

ProgramPtr Program::link(const std::vector<ShaderPtr>& shaders) {
  unsigned id = glCreateProgram();
  if (!id)
    errx(1, "Unable to link a program");

  ProgramPtr program(new Program(id));

  for (const ShaderPtr &shader : shaders)
    glAttachShader(id, shader->id);

  glLinkProgram(id);
  check_errors(id, glGetProgramiv, glGetProgramInfoLog,
               GL_LINK_STATUS, "Program linking");

  for (const ShaderPtr &shader : shaders)
    glDetachShader(id, shader->id);

  return program;
}

int Program::attrib(const char *name) const {
  int attr = glGetAttribLocation(id, name);
  if (attr == -1)
    errx(1, "Attribute %s not found", name);

  return attr;
}

int Program::uniform(const char *name) const {
  int uniform = glGetUniformLocation(id, name);
  if (uniform == -1)
    errx(1, "Uniform %s not found", name);

  return uniform;
}
