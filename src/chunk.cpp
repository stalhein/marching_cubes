#include "chunk.hpp"
#include <chrono>

glm::vec3 interpolateVertex(glm::vec3 p1, glm::vec3 p2, float d1, float d2) {
    float t = (ISO - d1) / (d2 - d1);
    return p1 + t * (p2 - p1);
}

glm::vec3 Chunk::getNormal(int x, int y, int z) {
    const int eps = 1;
    float dx = paddedDensities[idx(x+eps, y, z)] - paddedDensities[idx(x-eps, y, z)];
    float dy = paddedDensities[idx(x, y+eps, z)] - paddedDensities[idx(x, y-eps, z)];
    float dz = paddedDensities[idx(x, y, z+eps)] - paddedDensities[idx(x, y, z-eps)];

    return -glm::normalize(glm::vec3(dx, dy, dz));
}

Chunk::Chunk(glm::vec3 chunkPosition) : position(chunkPosition) {
    model = glm::translate(
        glm::mat4(1.0f),
        position * (float)SIZE
    );

    Logger::debug(std::format("Chunk at ({}, {}, {}): model matrix calculated", position.x, position.y, position.z));

    main = FastNoise::New<FastNoise::Simplex>();
    mainFractal = FastNoise::New<FastNoise::FractalFBm>();

    main->SetScale(800.f);
    mainFractal->SetSource(main);
    mainFractal->SetOctaveCount(4);
    
    Logger::debug(std::format("Chunk at ({}, {}, {}): noises created", position.x, position.y, position.z));


}

void Chunk::generate() {
    Logger::debug(std::format("Chunk ({}, {}, {})", position.x, position.y, position.z));
    int chunkX = position.x * SIZE;
    int chunkY = position.y * SIZE;
    int chunkZ = position.z * SIZE;

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel sections
    {
        #pragma omp section
        mainFractal->GenUniformGrid2D(heightMap, chunkX-1, chunkZ-1, SIZE+3, SIZE+3, 1.f, 1.f, 1234);
    }

    #pragma omp parallel for collapse(3) schedule(static)
    for (int x = 0; x <= SIZE+2; ++x) {
        for (int y = 0; y <= SIZE+2; ++y) {
            for (int z = 0; z <= SIZE+2; ++z) {
                float heightValue = (heightMap[z * (SIZE+3) + x]+1.f)*0.5f;

                float base = heightValue*WORLD_HEIGHT*0.4f + WORLD_HEIGHT * 0.3 - (chunkY+y);
                paddedDensities[idx(x-1, y-1, z-1)] = base;
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    double m = std::chrono::duration<double, std::milli>(end - start).count();
    Logger::debug(std::format("  Density sample generation {}ms", m));


    start = std::chrono::high_resolution_clock::now();
    vertices.reserve(SIZE*SIZE*SIZE*5);
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
                    paddedDensities[idx(x, y, z)],
                    paddedDensities[idx(x+1, y, z)],
                    paddedDensities[idx(x+1, y+1, z)],
                    paddedDensities[idx(x, y+1, z)],
                    paddedDensities[idx(x, y, z+1)],
                    paddedDensities[idx(x+1, y, z+1)],
                    paddedDensities[idx(x+1, y+1, z+1)],
                    paddedDensities[idx(x, y+1, z+1)]
                };
                glm::vec3 cornerGradients[8] = {
                    gradients[idxGradients(x, y, z)],
                    gradients[idxGradients(x+1, y, z)],
                    gradients[idxGradients(x+1, y+1, z)],
                    gradients[idxGradients(x, y+1, z)],
                    gradients[idxGradients(x, y, z+1)],
                    gradients[idxGradients(x+1, y, z+1)],
                    gradients[idxGradients(x+1, y+1, z+1)],
                    gradients[idxGradients(x, y+1, z+1)]
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
                    float f = (ISO - cornerDensities[a]) / (cornerDensities[b] - cornerDensities[a]);
                    edgeVertices[i] = cornerPositions[a] + f * (cornerPositions[b] - cornerPositions[a]);
                }

                for (int i = 0; triTable[cube][i] != -1; i+=3) {
                    int a = triTable[cube][i];
                    int b = triTable[cube][i+1];
                    int c = triTable[cube][i+2];

                    glm::vec3 v1 = {edgeVertices[a][0], edgeVertices[a][1], edgeVertices[a][2]};
                    glm::vec3 v2 = {edgeVertices[b][0], edgeVertices[b][1], edgeVertices[b][2]};
                    glm::vec3 v3 = {edgeVertices[c][0], edgeVertices[c][1], edgeVertices[c][2]};

                    glm::vec3 edge1 = v2 - v1;
                    glm::vec3 edge2 = v3 - v1;
                    glm::vec3 n = glm::normalize(glm::cross(edge1, edge2));

                    vertices.push_back(v1.x);
                    vertices.push_back(v1.y);
                    vertices.push_back(v1.z);

                    vertices.push_back(n.x);
                    vertices.push_back(n.y);
                    vertices.push_back(n.z);

                    vertices.push_back(v2.x);
                    vertices.push_back(v2.y);
                    vertices.push_back(v2.z);

                    vertices.push_back(n.x);
                    vertices.push_back(n.y);
                    vertices.push_back(n.z);
         
                    vertices.push_back(v3.x);
                    vertices.push_back(v3.y);
                    vertices.push_back(v3.z);
        
                    vertices.push_back(n.x);
                    vertices.push_back(n.y);
                    vertices.push_back(n.z);
                }
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    m = std::chrono::duration<double, std::milli>(end - start).count();
    Logger::debug(std::format("  Mesh generation           {}ms", m));


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
