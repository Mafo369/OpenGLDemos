#ifndef CAMERA_H
#define CAMERA_H
#include "../../Rendering/error_handling.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    LEFT = 0,
    FORWARD,
    RIGHT,
    BACKWARD
};


/*------------------------------------------------------------------------------------------------------------------------*/
/*                                                Abstract Camera                                                         */
/*------------------------------------------------------------------------------------------------------------------------*/

class Camera {

public:
    Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 1.f), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), glm::vec3 look = glm::vec3(0.f, 0.f, 0.f), float zoom=45.f);
    virtual ~Camera();

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 viewmatrix() const;
    float &zoom();
    glm::vec3 &position();

    void setviewport(glm::vec4 viewport);

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    virtual void processkeyboard(Camera_Movement direction, GLfloat deltaTime);
    // Processes input received from a mouse input system.
    virtual void processmouseclick(int button, GLfloat xpos, GLfloat ypos);
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    virtual void processmousemovement(int button, GLfloat xpos, GLfloat ypos, GLboolean constraint = true);
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    virtual void processmousescroll(GLfloat yoffset);

protected:
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _up;
    float _zoom;

    glm::vec4 _viewport;

    // mouse movement
    int _mousebutton;
    GLfloat _mousestartx;
    GLfloat _mousestarty;

};


/*------------------------------------------------------------------------------------------------------------------------*/
/*                                       Euler Camera : Yaw, Pitch, Roll                                                  */
/*------------------------------------------------------------------------------------------------------------------------*/

// from learnopenGL tutorial
// Default camera values
constexpr GLfloat YAW        = -90.0f;
constexpr GLfloat PITCH      =  0.0f;
constexpr GLfloat SPEED      =  3.5f;
constexpr GLfloat SENSITIVTY =  0.1f;
constexpr GLfloat ZOOM       =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class EulerCamera : public Camera {

public:

    // Constructor with vectors (default constructor)
    EulerCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);

    ~EulerCamera();

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processkeyboard(Camera_Movement direction, GLfloat deltaTime) override;

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processmousemovement(int button, GLfloat xpos, GLfloat ypos, GLboolean constrainPitch = true) override;


private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updatecameravectors();

    // Camera Attributes
    glm::vec3 _right;
    glm::vec3 _worldup;
    // Eular Angles
    GLfloat _yaw;
    GLfloat _pitch;
    // Camera options
    GLfloat _movementspeed;
    GLfloat _mousesensitivity;

};


/*------------------------------------------------------------------------------------------------------------------------*/
/*                                            Trackball Camera                                                            */
/*                                  Always rotate around point (0, 0, 0)                                                  */
/*                                      Accumulate error on roll                                                          */
/*------------------------------------------------------------------------------------------------------------------------*/
class TrackballCamera : public Camera {

public:

    // Constructor with vectors (default constructor)
    TrackballCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);

    ~TrackballCamera();

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processkeyboard(Camera_Movement direction, GLfloat deltaTime) override;

    // Processes input received from a mouse input system.
    void processmouseclick(int button, GLfloat xpos, GLfloat ypos) override;

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processmousemovement(int button, GLfloat xpos, GLfloat ypos, GLboolean constrainPitch = false) override;


private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updatecameravectors();

    // Camera Attributes
    glm::vec3 _right;
    glm::vec3 _worldup;
    // Eular Angles
    GLfloat _yaw;
    GLfloat _pitch;
    // Camera options
    GLfloat _movementspeed;
    GLfloat _mousesensitivity;

};

#endif // CAMERA_H
