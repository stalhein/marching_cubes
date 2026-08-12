#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraDirections {FORWARD, BACKWARD, UP, DOWN, RIGHT, LEFT };

constexpr float SPEED = 80.f;
constexpr float SENSITIVITY = 0.1f;

class Camera {
public:
  glm::vec3 position, front, frontXZ, right, up, worldUp;
  float yaw, pitch;

  Camera(glm::vec3 startPosition, float startYaw, float startPitch)
    : position(startPosition), yaw(startYaw), pitch(startPitch),
    front(glm::vec3(0.f)), frontXZ(glm::vec3(0.f)),
    up(glm::vec3(0.f, 1.f, 0.f)), worldUp(up) {
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix() {
    return glm::lookAt(position, position + front, up);
  }

  void processKeyboard(CameraDirections dir, float deltaTime) {
    float v = SPEED * deltaTime;

    switch(dir) {
      case FORWARD:
        position += frontXZ * v;
        break;
      case BACKWARD:
        position -= frontXZ * v;
        break;
      case UP:
        position += worldUp * v;
        break;
      case DOWN:
        position -= worldUp * v;
        break;
      case RIGHT:
        position += right * v;
        break;
      case LEFT:
        position -= right * v;
        break;
    }
  }

  void processMouse(float xoffset, float yoffset) {
    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.f) pitch = 89.f;
    else if (pitch < -89.f) pitch = -89.f;

    updateCameraVectors();
  }
private:
  void updateCameraVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = 0.f;
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontXZ = glm::normalize(f);
    f.y = sin(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }
};
