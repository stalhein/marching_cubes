#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <FastNoise/FastNoise.h>

#include <vector>

#include "settings.hpp"
#include "tables.hpp"


class Chunk {
public:
  glm::vec3 position;
  glm::mat4 model;

  Chunk(glm::vec3 chunkPosition);

  void generate();

  void render();

private:
  GLuint vbo, vao;
  float paddedDensities[(SIZE+3)*(SIZE+3)*(SIZE+3)];
  glm::vec3 gradients[(SIZE+1)*(SIZE+1)*(SIZE+1)];
  std::vector<float> vertices;
  FastNoise::SmartNode<FastNoise::Simplex> main;
  FastNoise::SmartNode<FastNoise::FractalRidged> mainFractal;
  FastNoise::SmartNode<FastNoise::Simplex> detail;
  FastNoise::SmartNode<FastNoise::FractalFBm> detailFractal;
  float heightMap[(SIZE+3)*(SIZE+3)];
  float detailDensities[(SIZE+3)*(SIZE+3)*(SIZE+3)];

  float getDensity(float x, float y, float z);
  glm::vec3 getNormal(int x, int y, int z);
  int idxGradients(int x, int y, int z) {
    return z * (SIZE+1) * (SIZE+1) + y * (SIZE+1) + x;
  }
  int idx(int x, int y, int z) {
    int nx = x+1;
    int ny = y+1;
    int nz = z+1;
    return nz * (SIZE+3) * (SIZE+3) + ny * (SIZE+3) + nx;
  }
};
