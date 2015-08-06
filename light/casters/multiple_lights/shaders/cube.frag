#version 330
#define NUM_P_LIGHTS 4

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

struct d_light {
  vec3 dir;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct p_light {
  vec3 pos;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float cst;
  float lin;
  float quad;
};

in vec3 Norm;
in vec3 FragPos;
in vec2 Tex;

uniform Material m;
uniform vec3 v_pos;

uniform p_light p_lights[NUM_P_LIGHTS];
uniform d_light d;

out vec4 color;

vec3 compute_d_lighting(d_light d, vec3 norm, vec3 v_dir);
vec3 compute_p_light(p_light p, vec3 norm, vec3 v_dir);

void main() {
  vec3 norm  = normalize(Norm);
  vec3 v_dir = normalize(v_pos - FragPos);

  vec3 result = compute_d_lighting(d, norm, v_dir);
  for(int i = 0; i < 1; i++)
    result += compute_p_light(p_lights[i], norm, v_dir); 

  color = vec4(result, 1.0f);
}

vec3 compute_d_lighting(d_light d, vec3 norm, vec3 v_dir) {
  vec3 l_dir = normalize(-d.dir);
  float diff = max(dot(norm, l_dir), 0.0);

  vec3 reflect_dir = reflect(-l_dir, norm);
  float spec = pow(max(dot(v_dir, reflect_dir), 0.0), m.shininess);

  vec3 ambient  = d.ambient * vec3(texture(m.diffuse, Tex));
  vec3 diffuse  = d.diffuse * diff * vec3(texture(m.diffuse, Tex));
  vec3 specular = d.specular * spec * vec3(texture(m.specular, Tex));

  return ambient + diffuse + specular;
}

vec3 compute_p_light(p_light p, vec3 norm, vec3 v_dir) {
    vec3 l_dir = normalize(p.pos - FragPos);

    float diff = max(dot(norm, l_dir), 0.0);
    vec3 reflect_dir = reflect(-l_dir, norm);
    float spec = pow(max(dot(v_dir, reflect_dir), 0.0), m.shininess);
    float dst = length(p.pos - FragPos);
    float attenuation = 1.0f /
                        (p.cst + p.lin * dst + p.quad * (dst * dst));

    vec3 ambient  = p.ambient * vec3(texture(m.diffuse, Tex));
    vec3 diffuse  = p.diffuse * diff * vec3(texture(m.diffuse, Tex));
    vec3 specular = p.specular * spec * vec3(texture(m.specular, Tex));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}
