#ifndef SCENE_H
#define SCENE_H


#include <vector>

#include "error_handling.h"
#include <QOpenGLFunctions_4_1_Core>

/** Simple class for managing an OpenGL demo
 */
class OpenGLDemo : public QOpenGLFunctions_4_1_Core {

public:
    explicit OpenGLDemo(int width, int height);
    virtual ~OpenGLDemo();

    virtual void resize(int width, int height);
    virtual void draw();

    virtual void mouseclick(int button, float xpos, float ypos);
    virtual void mousemove(float xpos, float ypos);
    virtual void mousewheel(float delta);
    virtual void keyboardmove(int key, double time);
    virtual bool keyboard(unsigned char k);


    void toggledrawmode();

protected:
    // Width and heigth of the viewport
    int _width;
    int _height;

private:
    // Rendering mode (true is filled, false is wireframed
    bool _drawfill;
};


#endif // SCENE_H
