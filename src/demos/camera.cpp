#include "camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtc/quaternion.hpp"
#include "gtx/norm.hpp"
#include "gtc/matrix_transform.hpp"

/*------------------------------------------------------------------------------------------------------------------------*/

Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 look, float zoom, float cameraNearPlane, float cameraFarPlane) : _position(position), _front(look-position), _up(up), _zoom(zoom), m_nearPlane(cameraNearPlane), m_farPlane(cameraFarPlane){

}

Camera::~Camera() {

}

glm::mat4 Camera::viewmatrix() const {

    return glm::lookAt(_position, _position + _front, _up);
}

void Camera::processkeyboard(Camera_Movement , GLfloat ) {

}

void Camera::processmouseclick(int button, GLfloat xpos, GLfloat ypos) {
    _mousebutton = button;
    _mousestartx = xpos;
    _mousestarty = ypos;
}

void Camera::processmousemovement(int , GLfloat , GLfloat , GLboolean ) {

}

void Camera::processmousescroll(GLfloat delta) {
    _position += delta*_front;
}

float &Camera::zoom() {
    return _zoom;
}

glm::vec3 &Camera::position() {
    return _position;
}

void Camera::setviewport(glm::vec4 viewport) {
    _viewport = viewport;
}


/*------------------------------------------------------------------------------------------------------------------------*/

// A camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
// Constructor with vectors
EulerCamera::EulerCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) :
            Camera(position, up, position+glm::vec3(0.0f, 0.0f, -1.0f), ZOOM),
            _movementspeed(SPEED), _mousesensitivity(SENSITIVTY) {
    _worldup = _up;
    _yaw = yaw;
    _pitch = pitch;
    updatecameravectors();
}

EulerCamera::~EulerCamera() {

}


// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void EulerCamera::processkeyboard(Camera_Movement direction, GLfloat deltaTime) {
    GLfloat velocity = _movementspeed * deltaTime;
    if (direction == FORWARD)
        _position += _front * velocity;
    if (direction == BACKWARD)
        _position -= _front * velocity;
    if (direction == LEFT)
        _position -= _right * velocity;
    if (direction == RIGHT)
        _position += _right * velocity;
}


// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void EulerCamera::processmousemovement(int button, GLfloat xpos, GLfloat ypos, GLboolean constrainPitch) {
    float xoffset = xpos - _mousestartx;
    float yoffset = _mousestarty - ypos;
    _mousestartx = xpos;
    _mousestarty = ypos;
    xoffset *= _mousesensitivity;
    yoffset *= _mousesensitivity;

    if (button == 2) {
        // change the zoom factor
        if (_zoom >= 1.0f && _zoom <= 45.0f)
            _zoom -= yoffset;
        if (_zoom <= 1.0f)
            _zoom = 1.0f;
        if (_zoom >= 45.0f)
            _zoom = 45.0f;
    } else {

        _yaw   += xoffset;
        _pitch += yoffset;
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (_pitch > 89.0f)
                _pitch = 89.0f;
            if (_pitch < -89.0f)
                _pitch = -89.0f;
        } else {
            if (_pitch > 89.0f)
                _pitch = -89.0f;
            if (_pitch < -89.0f)
                _pitch = 89.0f;
        }
        // Update Front, Right and Up Vectors using the updated Eular angles
        updatecameravectors();
    }
}


// Calculates the front vector from the Camera's (updated) Eular Angles
void EulerCamera::updatecameravectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = std::cos(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
    front.y = std::sin(glm::radians(_pitch));
    front.z = std::sin(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(_front, _worldup));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up    = glm::normalize(glm::cross(_right, _front));

}



/*------------------------------------------------------------------------------------------------------------------------*/
/*                                            Trackball Camera                                                            */
/*                                  Always rotate around point (0, 0, 0)                                                  */
/*                                      Accumulate error on roll                                                          */
/*------------------------------------------------------------------------------------------------------------------------*/
TrackballCamera::TrackballCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) :
    Camera(position, up, position+glm::vec3(0.0f, 0.0f, -1.0f), ZOOM),
    _movementspeed(SPEED), _mousesensitivity(SENSITIVTY) {
    _worldup = _up;
    _yaw = yaw;
    _pitch = pitch;
    updatecameravectors();
}

TrackballCamera::~TrackballCamera() {

}


// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void TrackballCamera::processkeyboard(Camera_Movement direction, GLfloat deltaTime) {
    GLfloat velocity = _movementspeed * deltaTime;
    if (direction == FORWARD)
        _position += _front * velocity;
    if (direction == BACKWARD)
        _position -= _front * velocity;
    if (direction == LEFT)
        _position -= _right * velocity;
    if (direction == RIGHT)
        _position += _right * velocity;
}

// Processes input received from a mouse input system.
void TrackballCamera::processmouseclick(int button, GLfloat xpos, GLfloat ypos) {
    Camera::processmouseclick( button, xpos, ypos);
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void TrackballCamera::processmousemovement(int button, GLfloat xpos, GLfloat ypos, GLboolean ) {
    float xoffset = xpos - _mousestartx;
    float yoffset = _mousestarty - ypos;
    _mousestartx = xpos;
    _mousestarty = ypos;
    xoffset *= _mousesensitivity;
    yoffset *= _mousesensitivity;

    if (button == 2) {
        // change the zoom factor
        if (_zoom >= 1.0f && _zoom <= 45.0f)
            _zoom -= yoffset;
        if (_zoom <= 1.0f)
            _zoom = 1.0f;
        if (_zoom >= 45.0f)
            _zoom = 45.0f;
    } else {

        _yaw   += xoffset;
        _pitch += yoffset;

        // Update Front, Right and Up Vectors using the updated Eular angles
        updatecameravectors();
    }
}


// Calculates the front vector from the Camera's (updated) Eular Angles
void TrackballCamera::updatecameravectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = std::cos(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
    front.y = std::sin(glm::radians(_pitch));
    front.z = std::sin(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(_front, _worldup));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up    = glm::normalize(glm::cross(_right, _front));
    _worldup = _up;

    float dist = glm::length(_position);
    _position =  _front * -dist;
}


