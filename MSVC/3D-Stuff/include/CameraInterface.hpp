#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraInterface
{
protected:
    CameraInterface();
    ~CameraInterface();

    enum class Direction {
        Left,
        Right,
        Forward,
        Backward,
        Up,
        Down
    };

    void rotateCamera(Direction dir, float degrees);
    void translateCamera(Direction dir);

protected:
    glm::vec4 _eye, _center, _up;
};

inline CameraInterface::CameraInterface()
    : _eye(0, 0, 5, 1),
    _center(0, 0, 0, 1),
    _up(0, 1, 0, 0)
{}

inline CameraInterface::~CameraInterface()
{}

inline void CameraInterface::rotateCamera(Direction dir, float degrees) {
}

inline void CameraInterface::translateCamera(Direction dir) {
    const float factor = .1f;

    auto transf = glm::mat4(1.0f);

    switch (dir) {
    case Direction::Left:
        transf = glm::translate(transf, glm::vec3(-1*factor, 0, 0));
        break;
    case Direction::Right:
        transf = glm::translate(transf, glm::vec3(1*factor, 0, 0));
        break;
    case Direction::Up:
        transf = glm::translate(transf, glm::vec3(0, 1*factor, 0));
        break;
    case Direction::Down:
        transf = glm::translate(transf, glm::vec3(0, -1*factor, 0));
        break;
    }

    _eye    = transf * _eye;
    //_center = transf * _center;
}