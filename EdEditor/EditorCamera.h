#pragma once

#include "RenderScene.h"

struct EditorCamera
{
    Camera CameraObject;

    glm::vec3 MovementDirection = glm::vec3(0.0f);
    glm::vec2 CameraRotationSpeed = glm::vec2(10.0f);
    f32 CameraSpeed = 2.0f; // M/S

    CameraElement ToElement() const;
};