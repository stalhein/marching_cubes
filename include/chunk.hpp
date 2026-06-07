#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

#include "settings.hpp"
#include "FastNoiseLite.hpp"
#include "tables.hpp"


class Chunk {
public:
  glm::vec3 position;
  glm::mat4 model;

  Chunk(glm::vec3 chunkPosition, FastNoiseLite *n);

  void generate(double *terrainTime, double *meshTime);

  void render();

private:
  GLuint vbo, vao;
  float densities[(SIZE+1)*(SIZE+1)*(SIZE+1)];
  glm::vec3 gradients[(SIZE+1)*(SIZE+1)*(SIZE+1)];
  std::vector<float> vertices;
  FastNoiseLite *noise;

  float getDensity(float x, float y, float z);
  glm::vec3 getNormal(int x, int y, int z);
  int idx(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x > SIZE || y > SIZE || z > SIZE) return 0;
    return z * (SIZE+1) * (SIZE+1) + y * (SIZE+1) + x;
  }
};
