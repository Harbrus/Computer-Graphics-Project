#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point lights being used in the scene
uniform point_light points[4];
// Spot lights being used in the scene
uniform spot_light spots[5];
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_pos;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 vertex_pos, in vec3 transformed_normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // *********************************
  // Get distance between point light and vertex
  float d = distance(point.position, vertex_pos);
  // Calculate attenuation factor
  float att_fat = point.constant + point.linear * d + point.quadratic * pow(d,2);
  // Calculate light colour
  vec4 light_col = (1/att_fat) * point.light_colour;
  light_col.a = 1.0f;
  // Calculate light dir
  vec3 light_dir = normalize(point.position - vertex_pos);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_col);
  vec3 half_v = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(transformed_normal, half_v), 0.0f), mat.shininess) * (mat.specular_reflection * light_col);
  vec4 primary = mat.emissive + diffuse;
  vec4 colourp = primary * tex_colour + specular;
  colourp.a = 1.0f;
  // *********************************
  return colourp;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 vertex_pos, in vec3 transformed_normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
  vec3 light_dir = normalize(spot.position - vertex_pos);
  // Calculate distance to light
  float d = distance(spot.position, vertex_pos);
  // Calculate attenuation value
  float att_fat = spot.constant + spot.linear * d + spot.quadratic * pow(d, 2);
  // Calculate spot light intensity
  float intensity = max(dot(-light_dir, spot.direction), 0.0f);
  // Calculate light colour
  vec4 light_col = ((pow(intensity, spot.power))/att_fat) * spot.light_colour;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_col);
  vec3 half_v = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(transformed_normal, half_v), 0.0f), mat.shininess) * (mat.specular_reflection * light_col);
  vec4 primary = mat.emissive + diffuse;
  vec4 colours  = primary * tex_colour + specular;
  colours.a = 1.0f;
  // ********************************* 
  return colours;
}

void main() {

  colour = vec4(0.0, 0.0, 0.0, 1.0);
  // *********************************
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vertex_pos);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);
  // Sum point lights
  for (int i = 0; i < 4; ++i)
  {
  colour += calculate_point(points[i], mat, vertex_pos, transformed_normal, view_dir, tex_colour);
  }
  // Sum spot lights
  for(int i = 0; i < 5; ++i)
  {
  colour += calculate_spot(spots[i], mat, vertex_pos, transformed_normal, view_dir, tex_colour);
  }
  // *********************************
}