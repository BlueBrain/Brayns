/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

#include <brayns/common/types.h>
#include <brayns/common/ui/Viewport.h>
#include <brayns/common/ui/manipulators/InspectCenterManipulator.h>
#include <brayns/common/ui/manipulators/FlyingModeManipulator.h>

namespace brayns
{

//! dedicated namespace for 3D glut viewer widget
/*! initialize everything GLUT-related */
void initGLUT(int32 *ac, const char **av);

/*! switch over to GLUT for control flow. This functoin will not return */
void runGLUT();

/*! helper class that allows for easily computing (smoothed) frame rate */
struct FPSCounter
{
    double smooth_nom;
    double smooth_den;
    double frameStartTime;

    FPSCounter()
    {
        smooth_nom = 0.;
        smooth_den = 0.;
        frameStartTime = 0.;
    }

    void startRender() { frameStartTime = std::time(0); }

    void doneRender()
    {
        const double seconds = std::time(0) - frameStartTime;
        smooth_nom = smooth_nom * 0.8f + seconds;
        smooth_den = smooth_den * 0.8f + 1.f;
    }

    double getFPS() const { return smooth_den / smooth_nom; }
};

class BaseWindow
{
public:

    static BaseWindow *_activeWindow;

    typedef enum
    {
        FRAMEBUFFER_COLOR,
        FRAMEBUFFER_DEPTH,
        FRAMEBUFFER_NONE
    } FrameBufferMode;

    typedef enum
    {
        MOVE_MODE           =(1<<0),
        INSPECT_CENTER_MODE =(1<<1)
    } ManipulatorMode;

    BaseWindow(int argc, const char **argv,
               const FrameBufferMode frameBufferMode,
               const ManipulatorMode initialManipulator=INSPECT_CENTER_MODE,
               int allowedManipulators=INSPECT_CENTER_MODE|MOVE_MODE);
    virtual ~BaseWindow();

    std::unique_ptr<InspectCenterManipulator> _inspectCenterManipulator;
    std::unique_ptr<FlyingModeManipulator> _flyingModeManipulator;

    /*! current manipulator */
    std::unique_ptr<AbstractManipulator> _manipulator;

    /*! size we'll create a window at */
    static Vector2i _defaultInitSize;

    /*! set a default camera position that views given bounds from the
        top left front */
    void setViewPort();

    /*! tell GLUT that this window is 'dirty' and needs redrawing */
    virtual void forceRedraw();

    /*! set window title */
    void setTitle(const char *title);

    /*! set window title */
    void setTitle(const std::string &title) { setTitle(title.c_str()); }

    /*! set viewport to given values */
    Viewport& getViewPort() { return _viewPort; }

    // ------------------------------------------------------------------
    // event handling - override this to change this widgets behavior
    // to input events
    // ------------------------------------------------------------------
    virtual void mouseButton(
            int32 which,
            bool released,
            const Vector2i& pos);

    virtual void motion(
            const Vector2i& pos);

    virtual void reshape( const Vector2i& newSize );

    virtual void idle();

    /*! display this window. By default this will just clear this
        window's framebuffer; it's up to the user to override this fct
        to do something more useful */
    virtual void display();

    // ------------------------------------------------------------------
    // helper functions
    // ------------------------------------------------------------------
    /*! activate _this_ window, in the sense that all future glut
        events get routed to this window instance */
    virtual void activate();

    void create(const char *title,
                size_t width, size_t height,
                bool fullScreen = false);

    /*! clear the frame buffer color and depth bits */
    void clearPixels();

    /*! draw uint32 pixels into the GLUT window (assumes window and buffer
     * dimensions are equal) */
    void drawPixels(const uint32 *framebuffer);

    /*! draw float4 pixels into the GLUT window (assumes window and buffer
     * dimensions are equal) */
    void drawPixels(const Vector3f *framebuffer);

    // ------------------------------------------------------------------
    // camera helper code
    // ------------------------------------------------------------------
    void snapUp();
    /*! set 'up' vector. if this vector is '0,0,0' the viewer will
       *not* apply the up-vector after camera manipulation */

    friend void glut3dReshape(int32 x, int32 y);
    friend void glut3dDisplay(void);
    friend void glut3dKeyboard(char key, int32 x, int32 y);
    friend void glut3dIdle(void);
    friend void glut3dMotionFunc(int32 x, int32 y);
    friend void glut3dMouseFunc(int32 whichButton, int32 released,
                                int32 x, int32 y);

    virtual void keypress(char key, const Vector2f& where);
    virtual void specialkey(int32 key, const Vector2f& where);

    /** Saves current frame to disk. The filename is defined by a prefix and a
     * frame index (<prefix>_<frame>_%08d.ppm). The file uses the ppm encoding
     * and is written to the working folder of the application.
     *
     * @param frameIndex index of the current frame
     * @param prefix prefix used for the filename
     */
    void saveFrameToDisk( size_t frameIndex, const std::string& prefix );

    void saveSceneToBinaryFile( const std::string& fn );
    void loadSceneFromBinaryFile( const std::string& fn );

    /*! last mouse screen position of mouse before current motion */
    Vector2i getLastMousePos() { return _lastMousePos; }

    /*! current screen position of mouse */
    Vector2i getCurrentMousePos() { return _currMousePos; }
    int64_t getLastButtonState() { return _lastButtonState; }
    int64_t getCurrentButtonState() { return _currButtonState; }
    int64_t getCurrentModifiers() { return _currModifiers; }
    Vector3f getUpVectorFromCmdLine() { return _upVectorFromCmdLine; }

    /*!< world bounds, to automatically set viewPort lookat, mouse speed, etc */
    Boxf getWorldBounds();

    float getRotateSpeed() { return _rotateSpeed; }
    float getMotionSpeed() { return _motionSpeed; }

    Vector2i getWindowSize() { return _windowSize; }

protected:

    Vector2i _lastMousePos; /*! last mouse screen position of mouse before
                                    current motion */
    Vector2i _currMousePos; /*! current screen position of mouse */
    u_int64_t _lastButtonState;
    u_int64_t _currButtonState;
    u_int64_t _currModifiers;
    Vector3f _upVectorFromCmdLine;

    /*! camera speed modifier - affects how many units the camera _moves_ with
     * each unit on the screen */
    float _motionSpeed;
    /*! camera rotation speed modifier - affects how many units the camera
     * _rotates_ with each unit on the screen */
    float _rotateSpeed;

    FrameBufferMode _frameBufferMode;

    Viewport _viewPort;
    int32 _windowID;
    Vector2i _windowSize;

    bool fullScreen_;
    int resampleSize_;
    int frameCounter_;

    FPSCounter _fps;

protected:
    BraynsPtr _brayns;

};

}

#endif
