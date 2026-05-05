#include "Camera.h"
#include <algorithm>
#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

void Camera::RotateCamera(float newX, float newY) {
    xPos -= newX * sensitivity;
    yPos = std::clamp(yPos+(newY * sensitivity), -89.0f, 89.0f);
}

void Camera::ZoomCamera(float newScale) {
    cameraZoom = std::clamp(cameraZoom-(newScale * sensitivity), 0.5f, 500.0f);
}

glm::vec3 Camera::getPosition() {
    float a = glm::radians(xPos);
    float b = glm::radians(yPos);

    return glm::vec3(cameraZoom * cos(b) * sin(a),
        cameraZoom * sin(b),
        cameraZoom * cos(b) * cos(a));
}

glm::mat4 Camera::getViewMatrix() {
    glm::vec3 pos = getPosition();
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    if (yPos > 89.0f || yPos < -89.0f) {
        upVector = glm::vec3(0.0f, 0.0f, (yPos>0)?-1.0f:1.0f);
    }

    return glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), upVector);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

