#ifndef SHADER_FACTORY_HH
# define SHADER_FACTORY_HH

# include <type_traits>
# include <vector>

# include "template/typelist.hh"
# include "shader/shader_factory_policy.hh"
# include "shader/structure_factory.hh"

template <typename TList>
struct ShaderFactory {
  ShaderFactory(const char *shader_code, unsigned shader_type);
  ~ShaderFactory();

  template <typename T>
  static ShaderFactory<T> from_file(const char *file, unsigned shader_type);
  char *generate();


  char *make_variable(const char *type, const char *name);
  char *compose();

  enum class Instructions {
    MVP = 0,
    Normal, Texture
  };

  template <typename T>
  struct Policy {
    /* Vertex shader */
    enum { mvp     = indexOf<T, SFP::MVP>::val    != -1 };
    enum { normal  = indexOf<T, SFP::Normal>::val != -1 };
    enum { texture = indexOf<T, SFP::TextureCoord>::val != -1 };

    /* Fragment shader */


    /* Material related */
    enum { material     = indexOf<T, SFP::Material>::val != -1 };
    enum { m_ambient    = indexOf<T, SFP::M_Ambient>::val != -1 };
    enum { m_diffuse    = indexOf<T, SFP::M_Diffuse>::val != -1 };
    enum { m_specular   = indexOf<T, SFP::M_Specular>::val != -1 };
    enum { m_emission   = indexOf<T, SFP::M_Emission>::val != -1 };
    enum { m_shininess  = indexOf<T, SFP::M_Shininess>::val != -1 };
    enum { m_ambeqdiff  =
             indexOf<T, SFP::M_AmbientEqualsDiffuse>::val != -1 };

    enum { m_tex_ambient    = indexOf<T, SFP::M_Tex_Ambient>::val != -1 };
    enum { m_tex_diffuse    = indexOf<T, SFP::M_Tex_Diffuse>::val != -1 };
    enum { m_tex_specular   = indexOf<T, SFP::M_Tex_Specular>::val != -1 };
    enum { m_tex_emission   = indexOf<T, SFP::M_Tex_Emission>::val != -1 };
    enum { m_tex_ambeqdiff  =
             indexOf<T, SFP::M_Tex_AmbientEqualsDiffuse>::val != -1 };

    /* Directional Light related */
    enum { dir_light   = indexOf<T, SFP::DirectionalLight>::val != -1 };
    enum { dl_ambient  = indexOf<T, SFP::DL_Ambient>::val != -1 };
    enum { dl_diffuse  = indexOf<T, SFP::DL_Diffuse>::val != -1 };
    enum { dl_specular = indexOf<T, SFP::DL_Specular>::val != -1 };
    enum { dl_pos      = indexOf<T, SFP::DL_Pos>::val != -1 };
  };

  #define INJECTOR_ENABLED(POLICY_VALUE)                     \
    typename std::enable_if<POLICY_VALUE>::type* = nullptr
  #define INJECTOR_DEFAULT(POLICY_VALUE)                     \
    typename std::enable_if<!(POLICY_VALUE)>::type* = nullptr

  #define INJECTOR(TYPE, NAME, POLICY_VALUE)     \
    template <typename T = TList>                \
    TYPE NAME(INJECTOR_ENABLED(POLICY_VALUE));   \
    template <typename T = TList>                \
    TYPE NAME(INJECTOR_DEFAULT(POLICY_VALUE)) {}

  #define P(ARG) Policy<T>::ARG

  INJECTOR(void, inject_mvp,            P(mvp));
  INJECTOR(void, inject_normal,         P(normal) && P(mvp));
  INJECTOR(void, inject_texture_coords, P(texture) && P(mvp));

  INJECTOR(void, inject_material, P(material));
  INJECTOR(void, inject_material_ambient,
                 P(material) && P(m_ambient));
  INJECTOR(void, inject_material_diffuse,
                 P(material) && P(m_diffuse));
  INJECTOR(void, inject_material_specular,
                 P(material) && P(m_specular));
  INJECTOR(void, inject_material_emission,
                 P(material) && P(m_emission));
  INJECTOR(void, inject_material_shininess,
                 P(material) && P(m_shininess));
  INJECTOR(void, inject_material_ambient_equal_diffuse,
                 P(material) && P(m_ambeqdiff));

  INJECTOR(void, inject_material_textured_ambient,
                 P(material) && P(m_tex_ambient));
  INJECTOR(void, inject_material_textured_diffuse,
                 P(material) && P(m_tex_diffuse));
  INJECTOR(void, inject_material_textured_specular,
                 P(material) && P(m_tex_specular));
  INJECTOR(void, inject_material_textured_emission,
                 P(material) && P(m_tex_emission));
  INJECTOR(void, inject_material_textured_ambient_equal_diffuse,
                 P(material) && P(m_tex_ambeqdiff));

  INJECTOR(void, inject_dl,          P(dir_light));
  INJECTOR(void, inject_dl_ambient,  P(dir_light) && P(dl_ambient));
  INJECTOR(void, inject_dl_diffuse,  P(dir_light) && P(dl_diffuse));
  INJECTOR(void, inject_dl_specular, P(dir_light) && P(dl_specular));
  INJECTOR(void, inject_dl_pos,      P(dir_light) && P(dl_pos));

  #undef P
  #undef INJECTOR
  #undef INJECTOR_DEFAULT
  #undef INJECTOR_ENABLED

  char *shader;
  unsigned shader_type;

  std::vector<StructureFactory *> structures;
  std::vector<char *> inputs;
  std::vector<char *> uniforms;
  std::vector<char *> outputs;

  std::vector<const char *> instructions;
};

#include "factory.hxx"


#endif
