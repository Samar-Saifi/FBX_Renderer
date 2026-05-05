#ifndef RENDERER_CAMERA_H
#define RENDERER_CAMERA_H
#include "glm/fwd.hpp"

class Camera {
private:
    float xPos = 0;
    float yPos = 0;
    float cameraZoom = 3;

    float sensitivity = 0.5f;

    float fov = 45.0f;
    float nearPlane = 0.01f;
    float farPlane = 1000;

public:
    void RotateCamera(float newX, float newY);
    void ZoomCamera(float newY);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float aspectRatio);
    glm::vec3 getPosition();
};


#endif //RENDERER_CAMERA_H
