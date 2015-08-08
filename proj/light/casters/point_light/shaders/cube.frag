#version 330

struct point_light {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

in vec3 Norm;
in vec3 FragPos;
in vec2 Tex;

uniform Material m;
uniform point_light p;
uniform vec3 view_position;
uniform float rand;

out vec4 color;

void main() {
  float distance    = length(p.position - FragPos);
  float attenuation = 1.0f / (p.constant + p.linear * distance +
                        p.quadratic * (distance * distance));

  vec3 ambient  = p.ambient * vec3(texture(m.diffuse, Tex));
  vec3 norm     = normalize(Norm);
  vec3 lightDir = normalize(p.position - FragPos);
  float diff    = max(dot(norm, lightDir), 0.0);
  vec3 diffuse  = p.diffuse * diff * vec3(texture(m.diffuse, Tex));

  float specularStrength = 0.5f;
  vec3 viewDir = normalize(view_position - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);
  vec3 specular = specularStrength * spec * vec3(texture(m.specular , Tex)) * p.specular;

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;

  float y = Tex.y + rand;
  if (y > 1) y -= 1;
  vec3 emission = vec3(texture(m.emission, vec2(Tex.x, y)));

  vec3 result = ambient + diffuse + specular + emission;
  color = vec4(result, 1.0f);
}
