#ifndef SHADER_FACTORY_POLICY_HH
# define SHADER_FACTORY_POLICY_HH

enum class ShaderFactoryPolicy {
  MVP, Normal, TextureCoord,

  Material,
    M_Ambient, M_Diffuse, M_Specular, M_Emission,
    M_Shininess, M_AmbientEqualsDiffuse,
    M_Tex_Ambient, M_Tex_Diffuse, M_Tex_Emission,
    M_Tex_Specular, M_Tex_AmbientEqualsDiffuse,

  DirectionalLight,
    DL_Ambient, DL_Diffuse, DL_Specular,
    DL_Pos
};

using SFP = enum ShaderFactoryPolicy;

#endif
