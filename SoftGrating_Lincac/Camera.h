
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"mat.h"

const float YAW = 90.0;
const float PITCH = 0.0;
const float ZOOM = 45.0;

class Camera {
public:
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;
    float Yaw;
    float Pitch;
    float Zoom;
    float camera_length;

    Camera() {};

    Camera(vec3 position = vec3(), vec3 front = vec3(0, 0, -1), vec3 up = vec3(0, 1, 0), float yaw = YAW, float pitch = PITCH, float zoom = ZOOM)
        : WorldUp()
    {
        Position = position;
        Front = front;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Zoom = zoom;
        camera_length = position.length();

        updateCameraVectors();
    };

    mat4 get_viewMatrix() { return lookAt(Position, vec3(0.0), Up); }
    void ProcessMouseScroll(float yoffset);
    void ProcessMouseButton(float xoffset, float yoffset);
protected:
    void updateCameraVectors();
};

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    Zoom = Math::clamp(1.0f, 45.0f, Zoom);
}

void Camera::ProcessMouseButton(float xoffset, float yoffset) {
    Yaw -= xoffset;
    Pitch -= yoffset;
    Pitch = Math::clamp(-89.0f, 89.0f, Pitch);

    Position.x = cos(Math::radians(Yaw)) * camera_length * cos(Math::radians(Pitch));
    Position.y = sin(Math::radians(Pitch)) * camera_length;
    Position.z = sin(Math::radians(Yaw)) * camera_length * cos(Math::radians(Pitch));

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    vec3 front;
    front.x = cos(Math::radians(Yaw)) * cos(Math::radians(Pitch));
    front.y = sin(Math::radians(Pitch));
    front.z = sin(Math::radians(Yaw)) * cos(Math::radians(Pitch));
    front.normalize();

    Front = front;
    Right = cross(Front, WorldUp);
    Right.normalize();
    Up = cross(Right, Front);
    Up.normalize();
}

#endif 