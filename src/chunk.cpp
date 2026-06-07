#include "chunk.hpp"

glm::vec3 interpolateVertex(glm::vec3 p1, glm::vec3 p2, float d1, float d2) {
  float t = (ISO - d1) / (d2 - d1);
  return p1 + t * (p2 - p1);
}

float Chunk::getDensity(float x, float y, float z) {
  return noise->GetNoise(x + position.x * SIZE, y + position.y * SIZE, z + position.z * SIZE) * 10.f;
}

glm::vec3 Chunk::getNormal(int x, int y, int z) {
  const int eps = 1;
  float dx = densities[idx(x+eps, y, z)] - densities[idx(x-eps, y, z)];
  float dy = densities[idx(x, y+eps, z)] - densities[idx(x, y-eps, z)];
  float dz = densities[idx(x, y, z+eps)] - densities[idx(x, y, z-eps)];

  return -glm::normalize(glm::vec3(dx, dy, dz));
}

Chunk::Chunk(glm::vec3 chunkPosition, FastNoiseLite *n) : position(chunkPosition), noise(n) {
  model = glm::translate(
    glm::mat4(1.0f),
    position * (float)SIZE
  );
}

void Chunk::generate(double *terrainTime, double *meshTime) {
  double now = glfwGetTime();
  for (int x = 0; x <= SIZE; ++x) {
    for (int y = 0; y <= SIZE; ++y) {
      for (int z = 0; z <= SIZE; ++z) {
        densities[idx(x, y, z)] = getDensity((float)x, (float)y, (float)z);
      }
    }
  }
  double next = glfwGetTime();
  *terrainTime += next - now;

  for (int x = 1; x < SIZE; ++x) {
    for (int y = 1; y < SIZE; ++y) {
      for (int z = 1; z < SIZE; ++z) {
        float dx = densities[idx(x+1, y, z)] - densities[idx(x-1, y, z)];
        float dy = densities[idx(x, y+1, z)] - densities[idx(x, y-1, z)];
        float dz = densities[idx(x, y, z+1)] - densities[idx(x, y, z-1)];

        gradients[idx(x, y, z)] = -glm::normalize(glm::vec3(dx, dy, dz));
      }
    }
  }

  vertices.reserve(100000);
  for (int x = 0; x < SIZE; ++x) {
    for (int y = 0; y < SIZE; ++y) {
      for (int z = 0; z < SIZE; ++z) {
        glm::vec3 cornerPositions[8] = {
          {x, y, z},
          {x+1, y, z},
          {x+1, y+1, z},
          {x, y+1, z},
          {x, y, z+1},
          {x+1, y, z+1},
          {x+1, y+1, z+1},
          {x, y+1, z+1}
        };
        float cornerDensities[8] = {
          densities[idx(x, y, z)],
          densities[idx(x+1, y, z)],
          densities[idx(x+1, y+1, z)],
          densities[idx(x, y+1, z)],
          densities[idx(x, y, z+1)],
          densities[idx(x+1, y, z+1)],
          densities[idx(x+1, y+1, z+1)],
          densities[idx(x, y+1, z+1)]
        };
        glm::vec3 cornerGradients[8] = {
          gradients[idx(x, y, z)],
          gradients[idx(x+1, y, z)],
          gradients[idx(x+1, y+1, z)],
          gradients[idx(x, y+1, z)],
          gradients[idx(x, y, z+1)],
          gradients[idx(x+1, y, z+1)],
          gradients[idx(x+1, y+1, z+1)],
          gradients[idx(x, y+1, z+1)]
        };

        int cube = 0;

        for (int i = 0; i < 8; ++i) {
          if (cornerDensities[i] < ISO) cube |= (1 << i);
        }

        int edges = edgeTable[cube];

        if (edges == 0) continue;

        glm::vec3 edgeVertices[12];
        glm::vec3 edgeNormals[12];
        for (int i = 0; i < 12; ++i) {
          if (!(edges & (1 << i))) continue;
          int a = edgeConnexions[i][0];
          int b = edgeConnexions[i][1];
          float f = (ISO - cornerDensities[a]) / (cornerDensities[b] - cornerDensities[a]);
          edgeVertices[i] = cornerPositions[a] + f * (cornerPositions[b] - cornerPositions[a]);
          edgeNormals[i] = glm::normalize(cornerGradients[a] + f * (cornerGradients[b] - cornerGradients[a]));
        }

        for (int i = 0; triTable[cube][i] != -1; i+=3) {
          int a = triTable[cube][i];
          int b = triTable[cube][i+1];
          int c = triTable[cube][i+2];

          glm::vec3 v1 = {edgeVertices[a][0], edgeVertices[a][1], edgeVertices[a][2]};
          glm::vec3 v2 = {edgeVertices[b][0], edgeVertices[b][1], edgeVertices[b][2]};
          glm::vec3 v3 = {edgeVertices[c][0], edgeVertices[c][1], edgeVertices[c][2]};

          glm::vec3 n1 = edgeNormals[a];
          glm::vec3 n2 = edgeNormals[b];
          glm::vec3 n3 = edgeNormals[c];


          vertices.push_back(v1.x);
          vertices.push_back(v1.y);
          vertices.push_back(v1.z);

          vertices.push_back(n1.x);
          vertices.push_back(n1.y);
          vertices.push_back(n1.z);

          vertices.push_back(v2.x);
          vertices.push_back(v2.y);
          vertices.push_back(v2.z);

          vertices.push_back(n2.x);
          vertices.push_back(n2.y);
          vertices.push_back(n2.z);
         
          vertices.push_back(v3.x);
          vertices.push_back(v3.y);
          vertices.push_back(v3.z);
        
          vertices.push_back(n3.x);
          vertices.push_back(n3.y);
          vertices.push_back(n3.z);
        }
      }
    }
    *meshTime += glfwGetTime() - next;
  }


  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

  glBindVertexArray(0);
}

void Chunk::render() {
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
  glBindVertexArray(0);
}
