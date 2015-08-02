#include <streambuf>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <err.h>
#include <epoxy/gl.h>

#include "shader/factory.hh"

using namespace std;

enum class Structures {
  Material = 0,
  Directional_Light
};

template <typename TList>
ShaderFactory<TList>::ShaderFactory(const char *shader_code,
                                    unsigned shader_type):
  shader(strdup(shader_code)), shader_type(shader_type)
{
  structures.push_back(new StructureFactory(NULL));
  structures.push_back(new StructureFactory(NULL));
}

template <typename TList>
ShaderFactory<TList>::~ShaderFactory() {
  free(shader);
  for (size_t i = 0; i < structures.size(); ++i)
    delete structures[i];

  for (size_t i = 0; i < inputs.size(); ++i)
    free(inputs[i]);
  for (size_t i = 0; i < uniforms.size(); ++i)
    free(uniforms[i]);
  for (size_t i = 0; i < outputs.size(); ++i)
    free(outputs[i]);
}


template <typename TList>
template <typename T>
ShaderFactory<T> ShaderFactory<TList>::from_file(const char *file,
                                                 unsigned shader_type) {

  std::ifstream t(file);
  std::string str((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

  return ShaderFactory<T>(str.c_str(), shader_type);
}

template <typename TList>
char *ShaderFactory<TList>::make_variable(const char *type, const char *name) {

  int len1 = strlen(type);
  int len2 = strlen(name);

  /*
   * We need to allocate the space for two more
   * chars:
   *   * the space between the type and the name
   *   * the semicolon at the end of the instruction
   * member = "type name;\0"
   */
  char *var = (char *) malloc(len1 + len2 + 3);

  strcpy(var, type);
  strcpy(var + len1 + 1, name);

  var[len1] = ' ';
  var[len1 + len2 + 1] = ';';
  var[len1 + len2 + 2] = '\0';

  return var;
}

template <typename TList>
char *ShaderFactory<TList>::generate() {
  inject_mvp();
  inject_normal();
  inject_texture_coords();

  inject_material();
  inject_material_ambient();
  inject_material_diffuse();
  inject_material_specular();
  inject_material_emission();
  inject_material_shininess();
  inject_material_ambient_equal_diffuse();

  inject_material_textured_ambient();
  inject_material_textured_diffuse();
  inject_material_textured_specular();
  inject_material_textured_emission();
  inject_material_textured_ambient_equal_diffuse();

  inject_dl();
  inject_dl_ambient();
  inject_dl_diffuse();
  inject_dl_specular();
  inject_dl_pos();

  return compose();
}

template <typename TList>
char *ShaderFactory<TList>::compose() {
  size_t len1 = strlen(shader);
  size_t structs_len = 0, inputs_len = 0, uniforms_len = 0,
         outputs_len = 0, instructions_len = 0;

  for (size_t i = 0; i < structures.size(); ++i)
    structs_len += structures[i]->len;

  for (size_t i = 0; i < inputs.size(); ++i)
    inputs_len += strlen(inputs[i]);
  for (size_t i = 0; i < uniforms.size(); ++i)
    uniforms_len += strlen(uniforms[i]);
  for (size_t i = 0; i < outputs.size(); ++i)
    outputs_len += strlen(outputs[i]);
  for (size_t i = 0; i < instructions.size(); ++i)
    instructions_len += strlen(instructions[i]);

  char *code = (char *) calloc (1, len1 + structs_len +
                       inputs_len + uniforms_len + outputs_len +
                       instructions_len);
  const char *begin = shader;

  // will crash if there is no \n
  if (!strncmp(shader, "#version", sizeof ("#version") - 1)) {
    begin = strchr(code, '\n');
    shader += (size_t) (begin - code);
  }

  size_t s = sizeof ("#version 330\n") - 1;
  strcpy(code, "#version 330\n");

  size_t len = 0;
  for (size_t i = 0; i < structures.size(); ++i) {
    size_t siz = 0;
    char *struc = structures[i]->generate(&siz);
    strcpy(code + s + len, struc);
    free(struc);
    len += siz;
  }

  for (size_t i = 0; begin != shader + len1 &&
       strncmp(begin, "void main", sizeof ("void main" ) -1);
       ++begin, ++i, s++)
    (code + s + len)[i] = *begin;

  strcpy(code + s + len, "void main {");
  s += sizeof ("void main {") - 1;

  for (size_t i = 0; i < instructions.size(); ++i) {
    strcpy(code + s + len, instructions[i]);
    s += strlen(instructions[i]);
  }

  strcpy(code + s + len, "}");

  return code;
}

#define INJECTOR(TYPE, NAME, ...)                          \
  template<typename TList>                                  \
  template<typename T>                                      \
  TYPE ShaderFactory<TList>::NAME(                          \
    typename enable_if<__VA_ARGS__>::type* \
  )

template <typename TList, typename T>
using SFP2 = typename ShaderFactory<TList>::template Policy<T>;

INJECTOR(void, inject_mvp, SFP2<TList, T>::mvp) {
  if (shader_type != GL_VERTEX_SHADER)
    errx(1, "Can only inject mvp in a vertex shader");

  inputs.push_back(make_variable("vec3", "vp"));

  uniforms.push_back(make_variable("mat4", "model"));
  uniforms.push_back(make_variable("mat4", "mvp"));

  instructions.push_back("gl_Position = mvp * vec4(vp, 1.0);");
}

INJECTOR(void, inject_normal,
         SFP2<TList, T>::normal && SFP2<TList, T>::mvp) {

  if (shader_type == GL_VERTEX_SHADER) {
    inputs.push_back(make_variable("vec3", "normal"));
    outputs.push_back(make_variable("vec3", "Normal"));

    instructions.push_back(
      "Normal = mat3(transpose(inverse(model))) * normal;"
    );
  } else
    inputs.push_back(make_variable("vec3", "Normal"));
}

INJECTOR(void, inject_texture_coords,
         SFP2<TList, T>::texture && SFP2<TList, T>::mvp) {

  if (shader_type == GL_VERTEX_SHADER) {
    inputs.push_back(make_variable("vec2", "tex"));
    outputs.push_back(make_variable("vec2", "Tex"));

    instructions.push_back(
      "TexCoord = tex;"
    );
  } else
    inputs.push_back(make_variable("vec2", "Tex"));
}

INJECTOR(void, inject_material, SFP2<TList, T>::material) {
  StructureFactory *s = new StructureFactory("Material");

  delete structures[(int) Structures::Material];
  structures[(int) Structures::Material] = s;
}

INJECTOR(void, inject_material_ambient,
    SFP2<TList, T>::material && SFP2<TList, T>::m_ambient) {
  structures[(int) Structures::Material]->add_member("vec3", "ambient");
}

INJECTOR(void, inject_material_diffuse,
    SFP2<TList, T>::material && SFP2<TList, T>::m_diffuse) {
  structures[(int) Structures::Material]->add_member("vec3", "diffuse");
}

INJECTOR(void, inject_material_specular,
    SFP2<TList, T>::material && SFP2<TList, T>::m_specular) {
  structures[(int) Structures::Material]->add_member("vec3", "specular");
}

INJECTOR(void, inject_material_emission,
    SFP2<TList, T>::material && SFP2<TList, T>::m_emission) {
  structures[(int) Structures::Material]->add_member("vec3", "emission");
}

INJECTOR(void, inject_material_shininess,
    SFP2<TList, T>::material && SFP2<TList, T>::m_shininess) {
  structures[(int) Structures::Material]->add_member("float", "shininess");
}

INJECTOR(void, inject_material_ambient_equal_diffuse,
    SFP2<TList, T>::material && SFP2<TList, T>::m_ambeqdiff) {
  structures[(int) Structures::Material]->add_member("vec3", "diffuse");
}

INJECTOR(void, inject_material_textured_ambient,
    SFP2<TList, T>::material && SFP2<TList, T>::m_tex_ambient) {
  structures[(int) Structures::Material]->add_member("sampler2D", "ambient");
}

INJECTOR(void, inject_material_textured_diffuse,
    SFP2<TList, T>::material && SFP2<TList, T>::m_tex_diffuse) {
  structures[(int) Structures::Material]->add_member("sampler2D", "diffuse");
}

INJECTOR(void, inject_material_textured_specular,
    SFP2<TList, T>::material && SFP2<TList, T>::m_tex_specular) {
  structures[(int) Structures::Material]->add_member("sampler2D", "specular");
}

INJECTOR(void, inject_material_textured_emission,
    SFP2<TList, T>::material && SFP2<TList, T>::m_tex_emission) {
  structures[(int) Structures::Material]->add_member("sampler2D", "emission");
}

INJECTOR(void, inject_material_textured_ambient_equal_diffuse,
    SFP2<TList, T>::material && SFP2<TList, T>::m_tex_ambeqdiff) {
  structures[(int) Structures::Material]->add_member("sampler2D", "diffuse");
}

INJECTOR(void, inject_dl, SFP2<TList, T>::dir_light) {
  StructureFactory *s = new StructureFactory("DirectionalLight");
  delete structures[(int) Structures::Directional_Light];
  structures[(int) Structures::Directional_Light] = s;

  if (shader_type == GL_FRAGMENT_SHADER)
    instructions.push_back(
      "FragPos = vec3(model * vec4(vp, 1.0f));"
    );
}

INJECTOR(void, inject_dl_ambient,
    SFP2<TList, T>::dir_light && SFP2<TList, T>::dl_ambient) {
  structures[(int) Structures::Directional_Light]->add_member("vec3", "ambient");
}

INJECTOR(void, inject_dl_diffuse,
    SFP2<TList, T>::dir_light && SFP2<TList, T>::dl_diffuse) {
  structures[(int) Structures::Directional_Light]->add_member("vec3", "diffuse");
}

INJECTOR(void, inject_dl_specular,
    SFP2<TList, T>::dir_light && SFP2<TList, T>::dl_specular) {
  structures[(int) Structures::Directional_Light]->add_member("vec3", "specular");
}

INJECTOR(void, inject_dl_pos,
    SFP2<TList, T>::dir_light && SFP2<TList, T>::dl_pos) {
  structures[(int) Structures::Directional_Light]->add_member("vec3", "pos");
}
