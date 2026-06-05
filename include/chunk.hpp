#pragma once

#include <glad/glad.h>
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

  Chunk(glm::vec3 chunkPosition);

  void generate();

  void render();

private:
  GLuint vbo, vao;
  float densities[SIZE+1][SIZE+1][SIZE+1];
  std::vector<float> vertices;
};
