#version 330 core

out vec4 FragColor;

in vec3 Normal;



const vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));

void main() {
  float diffuse = max(dot(normalize(Normal), lightDirection), 0.0);
  vec3 colour = vec3(0.2) + diffuse * vec3(0.8);
  FragColor = vec4(colour, 1.0);
}
