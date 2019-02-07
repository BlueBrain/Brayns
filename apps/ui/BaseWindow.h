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

#include <GL/glew.h>

#include <brayns/common/Timer.h>
#include <brayns/common/types.h>

namespace brayns
{
//! dedicated namespace for 3D glut viewer widget
/*! initialize everything GLUT-related */
void initGLUT(int* ac, const char** av);

/*! switch over to GLUT for control flow. This functoin will not return */
void runGLUT();

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

    virtual void reshape(Vector2ui newSize);

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

    void create(const char* title);

    /*! clear the frame buffer color and depth bits */
    void clearPixels();

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
    Vector2ui _getWindowSize() const;

    Brayns& _brayns;

    Vector2i _lastMousePos{-1, -1}; /*! last mouse screen position of mouse
                                     before current motion */
    Vector2i _currMousePos{-1, -1}; /*! current screen position of mouse */
    u_int64_t _lastButtonState{0};
    u_int64_t _currButtonState{0};
    u_int64_t _currModifiers{0};

    FrameBufferMode _frameBufferMode;

    int _windowID{-1};
    Vector2ui _windowSize{0, 0};

    Timer _timer;

    bool _displayHelp{false};
    bool _fullScreen{false};
    Vector2ui _windowPosition{0, 0};

    GLuint _fbTexture{0};

private:
    void _onExit();
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
} // namespace brayns

#endif
