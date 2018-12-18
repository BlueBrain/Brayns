/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include "BaseWindow.h"

#include <brayns/Brayns.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/log.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/manipulators/AbstractManipulator.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/SceneParameters.h>

#include <assert.h>

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <unistd.h>
#else
#include "GL/glut.h"
#include <GL/freeglut_ext.h>
#endif

namespace
{
const int GLUT_WHEEL_SCROLL_UP = 3;
const int GLUT_WHEEL_SCROLL_DOWN = 4;
}

namespace brayns
{
void runGLUT()
{
    glutMainLoop();
}

void initGLUT(int* ac, const char** av)
{
    glutInit(ac, (char**)av);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
}

// ------------------------------------------------------------------
// glut event handlers
// ------------------------------------------------------------------
void glut3dReshape(int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->reshape(Vector2i(x, y));
}

void glut3dDisplay(void)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->display();
}

void glut3dKeyboard(unsigned char key, int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->keypress(key, Vector2i(x, y));
}
void glut3dSpecial(int key, int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->specialkey(key, Vector2i(x, y));
}

void glut3dIdle(void)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->idle();
}
void glut3dMotionFunc(int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->motion(Vector2i(x, y));
}

void glut3dMouseFunc(int whichButton, int released, int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->mouseButton(whichButton, released,
                                               Vector2i(x, y));
}

void glut3dPassiveMouseFunc(int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->passiveMotion(Vector2i(x, y));
}

// ------------------------------------------------------------------
// Base window
// ------------------------------------------------------------------
/*! currently active window */
BaseWindow* BaseWindow::_activeWindow = nullptr;

BaseWindow::BaseWindow(Brayns& brayns, const FrameBufferMode frameBufferMode)
    : _brayns(brayns)
    , _lastMousePos(-1, -1)
    , _currMousePos(-1, -1)
    , _lastButtonState(0)
    , _currButtonState(0)
    , _currModifiers(0)
    , _frameBufferMode(frameBufferMode)
    , _windowID(-1)
    , _displayHelp(false)
    , _fullScreen(false)
{
    const auto motionSpeed = _brayns.getCameraManipulator().getMotionSpeed();
    BRAYNS_INFO << "Camera       :" << _brayns.getEngine().getCamera()
                << std::endl;
    BRAYNS_INFO << "Motion speed :" << motionSpeed << std::endl;
}

BaseWindow::~BaseWindow() = default;

void BaseWindow::mouseButton(const int button, const bool released,
                             const Vector2i& pos)
{
    if (pos != _currMousePos)
        motion(pos);
    _lastButtonState = _currButtonState;

    if (released)
        _currButtonState = _currButtonState & ~(1 << button);
    else
        _currButtonState = _currButtonState | (1 << button);
    _currModifiers = glutGetModifiers();

    if (_currModifiers & GLUT_ACTIVE_SHIFT && released)
    {
        const auto& windowSize = _brayns.getParametersManager()
                                     .getApplicationParameters()
                                     .getWindowSize();
        const auto& result = _brayns.getEngine().getRenderer().pick(
            {pos.x() / float(windowSize.x()),
             1.f - pos.y() / float(windowSize.y())});
        if (result.hit)
        {
            // updates position based on new target and current rotation
            _brayns.getCameraManipulator().rotate(
                result.pos, 0, 0, AbstractManipulator::AxisMode::localY);
        }
    }

    if (button == GLUT_WHEEL_SCROLL_UP || button == GLUT_WHEEL_SCROLL_DOWN)
    {
        // Wheel events are reported twice like a button click (press + release)
        if (released)
            return;
        const auto delta = (button == GLUT_WHEEL_SCROLL_UP) ? 1 : -1;
        _brayns.getCameraManipulator().wheel(pos, delta);
    }
}

void BaseWindow::motion(const Vector2i& pos)
{
    _currMousePos = pos;
    if (_currButtonState != _lastButtonState)
    {
        // some button got pressed; reset 'old' pos to new pos.
        _lastMousePos = _currMousePos;
        _lastButtonState = _currButtonState;
    }

    auto& manipulator = _brayns.getCameraManipulator();

    if ((_currButtonState == (1 << GLUT_RIGHT_BUTTON)) ||
        ((_currButtonState == (1 << GLUT_LEFT_BUTTON)) &&
         (_currModifiers & GLUT_ACTIVE_ALT)))
    {
        manipulator.dragRight(_currMousePos, _lastMousePos);
    }
    else if ((_currButtonState == (1 << GLUT_MIDDLE_BUTTON)) ||
             ((_currButtonState == (1 << GLUT_LEFT_BUTTON)) &&
              (_currModifiers & GLUT_ACTIVE_CTRL)))
    {
        manipulator.dragMiddle(_currMousePos, _lastMousePos);
    }
    else if (_currButtonState == (1 << GLUT_LEFT_BUTTON))
    {
        manipulator.dragLeft(_currMousePos, _lastMousePos);
    }

    _lastMousePos = _currMousePos;
}

void BaseWindow::passiveMotion(const Vector2i& pos)
{
    _mouse = pos;
}

void BaseWindow::idle()
{
    usleep(1000);
}

void BaseWindow::reshape(Vector2ui newSize)
{
    auto& applicationParameters =
        _brayns.getParametersManager().getApplicationParameters();

    // In case of 3D stereo vision, make sure the width is even
    if (applicationParameters.isStereo())
    {
        if (newSize.x() % 2 != 0)
            newSize.x() = (newSize.x() - 1) / 2;
        else
            newSize.x() /= 2;
    }

    _windowSize = newSize;
    applicationParameters.setWindowSize(newSize);
}

void BaseWindow::activate()
{
    _activeWindow = this;
    glutSetWindow(_windowID);
}

void BaseWindow::forceRedraw()
{
    glutPostRedisplay();
}

void BaseWindow::display()
{
    _timer.start();
    _brayns.commitAndRender();

    const auto& newWindowSize = _getWindowSize();
    if (newWindowSize != _windowSize)
        glutReshapeWindow(newWindowSize.x(), newWindowSize.y());

    size_t offset = 0;
    for (auto frameBuffer : _brayns.getEngine().getFrameBuffers())
    {
        GLenum format = GL_RGBA;
        switch (frameBuffer->getFrameBufferFormat())
        {
        case FrameBufferFormat::bgra_i8:
            format = GL_BGRA;
            break;
        case FrameBufferFormat::rgb_i8:
            format = GL_RGB;
            break;
        default:
            format = GL_RGBA;
        }

        const auto& frameSize = frameBuffer->getFrameSize();

        frameBuffer->map();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, frameSize.x(), 0.0f, frameSize.y(), -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(offset, 0, frameSize.x(), frameSize.y());

        GLenum type = GL_FLOAT;
        const GLvoid* buffer = 0;
        switch (_frameBufferMode)
        {
        case FrameBufferMode::COLOR:
            type = GL_UNSIGNED_BYTE;
            buffer = frameBuffer->getColorBuffer();
            break;
        case FrameBufferMode::DEPTH:
            format = GL_LUMINANCE;
            buffer = frameBuffer->getDepthBuffer();
            break;
        default:
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        if (buffer)
        {
            glBindTexture(GL_TEXTURE_2D, _fbTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, frameBuffer->getSize().x(),
                         frameBuffer->getSize().y(), 0, format, type, buffer);

            glBegin(GL_QUADS);
            glTexCoord2f(0.f, 0.f);
            glVertex3f(0, 0, 0);
            glTexCoord2f(0.f, 1.f);
            glVertex3f(0, frameSize.y(), 0);
            glTexCoord2f(1.f, 1.f);
            glVertex3f(frameSize.x(), frameSize.y(), 0);
            glTexCoord2f(1.f, 0.f);
            glVertex3f(frameSize.x(), 0, 0);
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        frameBuffer->unmap();
        offset += frameSize.x();
    }

    if (_displayHelp)
    {
        auto& keyHandler = _brayns.getKeyboardHandler();
        glLogicOp(GL_XOR);
        glEnable(GL_COLOR_LOGIC_OP);
        _renderBitmapString(-0.98f, 0.95f, keyHandler.help());
        glDisable(GL_COLOR_LOGIC_OP);
    }

    _timer.stop();

    glutSwapBuffers();

    clearPixels();

    forceRedraw();
}

void BaseWindow::clearPixels()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BaseWindow::setTitle(const char* title)
{
    assert(_windowID >= 0);
    glutSetWindow(_windowID);
    glutSetWindowTitle(title);
}

void BaseWindow::create(const char* title)
{
    const auto windowSize = _getWindowSize();
    glutInitWindowSize(windowSize.x(), windowSize.y());
    _windowID = glutCreateWindow(title);
    _activeWindow = this;
    glutDisplayFunc(glut3dDisplay);
    glutReshapeFunc(glut3dReshape);
    glutKeyboardFunc(glut3dKeyboard);
    glutSpecialFunc(glut3dSpecial);
    glutMotionFunc(glut3dMotionFunc);
    glutMouseFunc(glut3dMouseFunc);
    glutPassiveMotionFunc(glut3dPassiveMouseFunc);
    glutIdleFunc(glut3dIdle);

    _registerKeyboardShortcuts();

    glGenTextures(1, &_fbTexture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _fbTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BaseWindow::keypress(const char key, const Vector2f&)
{
    switch (key)
    {
    case 'h':
        _displayHelp = !_displayHelp;
        break;
    case 27:
    case 'Q':
        _onExit();
#ifdef __APPLE__
        exit(0);
#else
        glutLeaveMainLoop();
#endif
        break;
    default:
        _brayns.getKeyboardHandler().handleKeyboardShortcut(key);
    }

    _brayns.getEngine().commit();
}

void BaseWindow::specialkey(const int key, const Vector2f&)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        _brayns.getKeyboardHandler().handle(SpecialKey::LEFT);
        break;
    case GLUT_KEY_RIGHT:
        _brayns.getKeyboardHandler().handle(SpecialKey::RIGHT);
        break;
    case GLUT_KEY_UP:
        _brayns.getKeyboardHandler().handle(SpecialKey::UP);
        break;
    case GLUT_KEY_DOWN:
        _brayns.getKeyboardHandler().handle(SpecialKey::DOWN);
        break;
    case GLUT_KEY_F11:
        if (_fullScreen)
            glutPositionWindow(_windowPosition.x(), _windowPosition.y());
        else
        {
            _windowPosition.x() = glutGet((GLenum)GLUT_WINDOW_X);
            _windowPosition.y() = glutGet((GLenum)GLUT_WINDOW_Y);
            glutFullScreen();
        }
        _fullScreen = !_fullScreen;
        break;
    }
}

void BaseWindow::_registerKeyboardShortcuts()
{
    auto& keyHandler = _brayns.getKeyboardHandler();
    keyHandler.registerKeyboardShortcut(
        'z', "Switch between depth and color buffers",
        std::bind(&BaseWindow::_toggleFrameBuffer, this));
}

#ifdef __APPLE__
void BaseWindow::_renderBitmapString(const float, const float,
                                     const std::string&)
{
}
#else
void BaseWindow::_renderBitmapString(const float x, const float y,
                                     const std::string& text)
{
    glRasterPos3f(x, y, 0.f);
    glutBitmapString(GLUT_BITMAP_8_BY_13,
                     reinterpret_cast<const unsigned char*>(text.c_str()));
    glRasterPos3f(-1.f, -1.f, 0.f);
}
#endif

Vector2ui BaseWindow::_getWindowSize() const
{
    Vector2ui newWindowSize;
    for (auto frameBuffer : _brayns.getEngine().getFrameBuffers())
    {
        newWindowSize.x() += frameBuffer->getFrameSize().x();
        newWindowSize.y() =
            std::max(newWindowSize.y(), frameBuffer->getFrameSize().y());
    }
    return newWindowSize;
}

void BaseWindow::_onExit()
{
    if (_fbTexture)
    {
        glDeleteTextures(1, &_fbTexture);
        _fbTexture = 0;
    }
}

void BaseWindow::_toggleFrameBuffer()
{
    if (_frameBufferMode == FrameBufferMode::DEPTH)
        _frameBufferMode = FrameBufferMode::COLOR;
    else
        _frameBufferMode = FrameBufferMode::DEPTH;
}
}
