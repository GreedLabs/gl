#ifndef MATERIAL_HH
# define MATERIAL_HH

# include "program.hh"
# include "texture.hh"

template <typename T>
struct Material;

template <>
struct Material<TexturePtr> {
  TexturePtr diffuse;
  TexturePtr specular;
  float shininess;

  inline void uniform(ProgramPtr p, const char *attr, TexturePtr value) {
    glUniform1i(p->uniform(attr), value->unit);
  }
};

template <>
struct Material<glm::vec3> {
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  inline void uniform(ProgramPtr p, const char *attr, glm::vec3 value) {
    glUniform3f(p->uniform(attr), value.x, value.y, value.z);
  }
};

#endif
