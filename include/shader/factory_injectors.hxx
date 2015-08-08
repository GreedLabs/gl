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

#define INJECTOR(TYPE, NAME, ...)                          \
  template<typename TList>                                 \
  template<typename T>                                     \
  TYPE ShaderFactory<TList>::NAME(                         \
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
    outputs.push_back(make_variable("vec3", "Norm"));

    instructions.push_back(
      "Norm = mat3(transpose(inverse(model))) * normal;"
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
      "Tex = tex;"
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
  if (shader_type == GL_VERTEX_SHADER) {
    outputs.push_back(make_variable("vec3", "FragPos"));
    instructions.push_back(
      "FragPos = vec3(model * vec4(vp, 1.0f));"
    );
  } else {
    StructureFactory *s = new StructureFactory("DirectionalLight");
    delete structures[(int) Structures::Directional_Light];
    structures[(int) Structures::Directional_Light] = s;
  }
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
