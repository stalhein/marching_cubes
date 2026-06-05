#include "chunk.hpp"

glm::vec3 interpolateVertex(glm::vec3 p1, glm::vec3 p2, float d1, float d2) {
  float t = (ISO - d1) / (d2 - d1);
  return p1 + t * (p2 - p1);
}

Chunk::Chunk(glm::vec3 chunkPosition) : position(chunkPosition) {
  model = glm::translate(
    glm::mat4(1.0f),
    position * (float)SIZE
  );
}

void Chunk::generate() {
  FastNoiseLite nosie;
  nosie.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  nosie.SetFrequency(0.02f);
  nosie.SetFractalOctaves(2);
  nosie.SetFractalType(FastNoiseLite::FractalType_FBm);

  for (int x = 0; x <= SIZE; ++x) {
    for (int y = 0; y <= SIZE; ++y) {
      for (int z = 0; z <= SIZE; ++z) {
        float height = nosie.GetNoise((float)x + position.x * SIZE, (float)z + position.z * SIZE) * 10.f + 16.f;
        densities[x][y][z] = height-y;
      }
    }
  }

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
          densities[x][y][z],
          densities[x+1][y][z],
          densities[x+1][y+1][z],
          densities[x][y+1][z],
          densities[x][y][z+1],
          densities[x+1][y][z+1],
          densities[x+1][y+1][z+1],
          densities[x][y+1][z+1]
        };

        int cube = 0;

        for (int i = 0; i < 8; ++i) {
          if (cornerDensities[i] < ISO) cube |= (1 << i);
        }

        int edges = edgeTable[cube];

        if (edges == 0) continue;

        glm::vec3 edgeVertices[12];
        for (int i = 0; i < 12; ++i) {
          if (!(edges & (1 << i))) continue;
          int a = edgeConnexions[i][0];
          int b = edgeConnexions[i][1];
          edgeVertices[i] = interpolateVertex(cornerPositions[a], cornerPositions[b],
              cornerDensities[a], cornerDensities[b]);
        }

        for (int i = 0; triTable[cube][i] != -1; i+=3) {
          int a = triTable[cube][i];
          int b = triTable[cube][i+1];
          int c = triTable[cube][i+2];

          glm::vec3 v1 = {edgeVertices[a][0], edgeVertices[a][1], edgeVertices[a][2]};
          glm::vec3 v2 = {edgeVertices[b][0], edgeVertices[b][1], edgeVertices[b][2]};
          glm::vec3 v3 = {edgeVertices[c][0], edgeVertices[c][1], edgeVertices[c][2]};

          glm::vec3 e1 = v2 - v1;
          glm::vec3 e2 = v3 - v1;

          glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

          vertices.push_back(v1.x);
          vertices.push_back(v1.y);
          vertices.push_back(v1.z);

          vertices.push_back(normal.x);
          vertices.push_back(normal.y);
          vertices.push_back(normal.z);

          vertices.push_back(v2.x);
          vertices.push_back(v2.y);
          vertices.push_back(v2.z);

          vertices.push_back(normal.x);
          vertices.push_back(normal.y);
          vertices.push_back(normal.z);
         
          vertices.push_back(v3.x);
          vertices.push_back(v3.y);
          vertices.push_back(v3.z);
        
          vertices.push_back(normal.x);
          vertices.push_back(normal.y);
          vertices.push_back(normal.z);
        }
      }
    }
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
  glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
  glBindVertexArray(0);
}
