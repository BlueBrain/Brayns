/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#ifdef __linux__
#include <unistd.h>
#endif

#include <apps/ui/gl/ScreenSpaceProcessor.h>
#include <brayns/common/types.h>

#include <chrono>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace brayns
{
//! dedicated namespace for 3D glut viewer widget
/*! initialize everything GLUT-related */
void initGLUT(int* ac, const char** av);

/*! switch over to GLUT for control flow. This functoin will not return */
void runGLUT();

/*! helper class that allows for easily computing (smoothed) frame rate */
struct FPSCounter
{
    high_resolution_clock::time_point _startTime;
    uint64_t _duration;

    void start() { _startTime = high_resolution_clock::now(); }
    void stop()
    {
        _duration = duration_cast<milliseconds>(high_resolution_clock::now() -
                                                _startTime)
                        .count();
    }

    float getFPS() { return 1000.f / _duration; }
};

/**
 * The different types of frame buffer targets for rendering.
 */
enum class FrameBufferMode
{
    COLOR,
    DEPTH
};

class BaseWindow
{
public:
    BaseWindow(Brayns& brayns,
               FrameBufferMode frameBufferMode = FrameBufferMode::COLOR);
    virtual ~BaseWindow();

    /*! size we'll create a window at */
    static Vector2i _defaultInitSize;

    /*! tell GLUT that this window is 'dirty' and needs redrawing */
    virtual void forceRedraw();

    /*! set window title */
    void setTitle(const char* title);

    /*! set window title */
    void setTitle(const std::string& title) { setTitle(title.c_str()); }
    // ------------------------------------------------------------------
    // event handling - override this to change this widgets behavior
    // to input events
    // ------------------------------------------------------------------
    virtual void mouseButton(int button, bool released, const Vector2i& pos);

    virtual void motion(const Vector2i& pos);

    virtual void passiveMotion(const Vector2i& pos);

    virtual void reshape(const Vector2i& newSize);

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

    void create(const char* title, size_t width, size_t height);

    /*! clear the frame buffer color and depth bits */
    void clearPixels();

    /*! draw uint pixels into the GLUT window (assumes window and buffer
     * dimensions are equal) */
    void drawPixels(const int* framebuffer);

    /*! draw float4 pixels into the GLUT window (assumes window and buffer
     * dimensions are equal) */
    void drawPixels(const Vector3f* framebuffer);

    virtual void keypress(char key, const Vector2f& where);
    virtual void specialkey(int key, const Vector2f& where);

    /** Saves current frame to disk. The filename is defined by a prefix and a
     * frame index (\<prefix\>_\<frame\>_%08d.ppm). The file uses the ppm
     * encoding
     * and is written to the working folder of the application.
     *
     * @param frameIndex index of the current frame
     * @param prefix prefix used for the filename
     */
    void saveFrameToDisk(size_t frameIndex, const std::string& prefix);

    void saveSceneToBinaryFile(const std::string& fn);
    void loadSceneFromBinaryFile(const std::string& fn);

protected:
    virtual void _registerKeyboardShortcuts();
    void _renderBitmapString(float x, float y, const std::string& text);

    Brayns& _brayns;

    Vector2i _lastMousePos; /*! last mouse screen position of mouse before
                                    current motion */
    Vector2i _currMousePos; /*! current screen position of mouse */
    Vector2i _mouse;
    u_int64_t _lastButtonState;
    u_int64_t _currButtonState;
    u_int64_t _currModifiers;

    FrameBufferMode _frameBufferMode;

    int _windowID;
    Vector2ui _windowSize;

    FPSCounter _fps;

    ScreenSpaceProcessor _screenSpaceProcessor;

    uint64_t _gid;

    bool _displayHelp;
    bool _fullScreen;
    Vector2ui _windowPosition;

private:
    void _exitApplication();
    void _toggleFrameBuffer();

    // ------------------------------------------------------------------
    // GLUT camera helper code
    // ------------------------------------------------------------------
    static BaseWindow* _activeWindow;
    friend void glut3dReshape(int x, int y);
    friend void glut3dDisplay(void);
    friend void glut3dKeyboard(unsigned char key, int x, int y);
    friend void glut3dSpecial(int key, int x, int y);
    friend void glut3dIdle(void);
    friend void glut3dMotionFunc(int x, int y);
    friend void glut3dMouseFunc(int whichButton, int released, int x, int y);
    friend void glut3dPassiveMouseFunc(int x, int y);
};
}

#endif
