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

#include "BaseWindow.h"

#include <brayns/Brayns.h>
#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/SceneParameters.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/camera/AbstractManipulator.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/input/KeyboardHandler.h>

#include <assert.h>

#ifdef __APPLE__
#  include "GLUT/glut.h"
#  include <unistd.h>
#else
#  include "GL/glut.h"
#  include <GL/freeglut_ext.h>
#endif

namespace
{
const float DEFAULT_MOTION_ACCELERATION = 1.5f;
const int GLUT_WHEEL_SCROLL_UP = 3;
const int GLUT_WHEEL_SCROLL_DOWN = 4;
}

namespace brayns
{

void runGLUT( )
{
    glutMainLoop( );
}

void initGLUT(int *ac, const char **av)
{
    glutInit( ac, (char **) av);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
}

// ------------------------------------------------------------------
// glut event handlers
// ------------------------------------------------------------------
void glut3dReshape(int x, int y)
{
    if (BaseWindow::_activeWindow)
        BaseWindow::_activeWindow->reshape(Vector2i(x,y));
}

void glut3dDisplay( void )
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->display( );
}

void glut3dKeyboard(unsigned char key, int x, int y)
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->keypress(key,Vector2i(x,y));
}
void glut3dSpecial(int key, int x, int y)
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->specialkey(key,Vector2i(x,y));
}

void glut3dIdle( void )
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->idle( );
}
void glut3dMotionFunc(int x, int y)
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->motion(Vector2i(x,y));
}

void glut3dMouseFunc(int whichButton, int released, int x, int y)
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->mouseButton(
                   whichButton, released, Vector2i(x,y));
}

void glut3dPassiveMouseFunc(int x, int y)
{
    if(BaseWindow::_activeWindow)
       BaseWindow::_activeWindow->passiveMotion( Vector2i(x,y) );
}

// ------------------------------------------------------------------
// Base window
// ------------------------------------------------------------------
/*! currently active window */
BaseWindow* BaseWindow::_activeWindow = nullptr;

BaseWindow::BaseWindow( BraynsPtr brayns, const FrameBufferMode frameBufferMode)
  : _brayns(brayns)
  , _lastMousePos(-1,-1)
  , _currMousePos(-1,-1)
  , _lastButtonState(0)
  , _currButtonState(0)
  , _currModifiers(0)
  , _frameBufferMode(frameBufferMode)
  , _windowID(-1)
  , _windowSize(-1,-1)
  , _displayHelp( false )
{
    const auto motionSpeed = _brayns->getCameraManipulator().getMotionSpeed();
    BRAYNS_INFO << "Camera       :" << _brayns->getCamera() << std::endl;
    BRAYNS_INFO << "Motion speed :" << motionSpeed << std::endl;
}

BaseWindow::~BaseWindow( )
{
}

void BaseWindow::mouseButton(
        const int button,
        const bool released,
        const Vector2i& pos)
{
    if(pos != _currMousePos)
        motion(pos);
    _lastButtonState = _currButtonState;

    if(released)
        _currButtonState = _currButtonState & ~(1<<button);
    else
        _currButtonState = _currButtonState |  (1<<button);
    _currModifiers = glutGetModifiers( );

    if( button == GLUT_WHEEL_SCROLL_UP || button == GLUT_WHEEL_SCROLL_DOWN )
    {
        // Wheel events are reported twice like a button click (press + release)
        if( released )
            return;
        const auto delta = (button == GLUT_WHEEL_SCROLL_UP) ? 1 : -1;
        _brayns->getCameraManipulator().wheel( pos, delta );
    }
}

void BaseWindow::motion(const Vector2i& pos)
{
    _currMousePos = pos;
    if( _currButtonState != _lastButtonState )
    {
        // some button got pressed; reset 'old' pos to new pos.
        _lastMousePos = _currMousePos;
        _lastButtonState = _currButtonState;
    }

    auto& manipulator = _brayns->getCameraManipulator();

    if(( _currButtonState == (1 << GLUT_RIGHT_BUTTON )) ||
      (( _currButtonState == ( 1 << GLUT_LEFT_BUTTON )) &&
      ( _currModifiers & GLUT_ACTIVE_ALT )))
    {
        manipulator.dragRight( _currMousePos, _lastMousePos );
    }
    else if(( _currButtonState == ( 1 << GLUT_MIDDLE_BUTTON )) ||
           (( _currButtonState == ( 1 << GLUT_LEFT_BUTTON )) &&
           ( _currModifiers & GLUT_ACTIVE_CTRL )))
    {
        manipulator.dragMiddle( _currMousePos, _lastMousePos );
    }
    else if( _currButtonState == ( 1 << GLUT_LEFT_BUTTON ))
    {
        manipulator.dragLeft( _currMousePos, _lastMousePos );
    }

    _lastMousePos = _currMousePos;
}

void BaseWindow::passiveMotion(const Vector2i& pos)
{
    _mouse = pos;
}

void BaseWindow::idle( )
{
    usleep(1000);
}

void BaseWindow::reshape(const Vector2i& newSize)
{
    _windowSize = newSize;
    _brayns->getCamera().setAspectRatio(float(newSize.x( ))/float(newSize.y( )));
    _brayns->reshape(newSize);
    _brayns->getParametersManager().getApplicationParameters().setWindowSize(newSize);

    if( !_brayns->getParametersManager().getApplicationParameters().getFilters().empty( ))
        _screenSpaceProcessor.resize( newSize.x( ), newSize.y( ));

    forceRedraw( );
}

void BaseWindow::activate( )
{
    _activeWindow = this;
    glutSetWindow(_windowID);
}

void BaseWindow::forceRedraw( )
{
    glutPostRedisplay( );
}

void BaseWindow::display( )
{
    const auto& camera = _brayns->getCamera();
    if( camera.getModified( ))
    {
        _brayns->getFrameBuffer().clear();
        _brayns->getCamera().resetModified();
    }

    _fps.start();

    RenderInput renderInput;
    RenderOutput renderOutput;

    renderInput.windowSize = _windowSize;
    renderInput.position = camera.getPosition();
    renderInput.target = camera.getTarget();
    renderInput.up = camera.getUp();

    _brayns->getCamera().commit();
    _brayns->render( renderInput, renderOutput );

    if( _brayns->getParametersManager().getApplicationParameters().getFilters().empty( ))
    {
        GLenum format = GL_RGBA;
        GLenum type   = GL_FLOAT;
        GLvoid* buffer = 0;
        switch(_frameBufferMode)
        {
        case FrameBufferMode::COLOR:
            type = GL_UNSIGNED_BYTE;
            buffer = renderOutput.colorBuffer.data( );
            break;
        case FrameBufferMode::DEPTH:
            format = GL_LUMINANCE;
            buffer = renderOutput.depthBuffer.data( );
            break;
        default:
            glClearColor( 0.f, 0.f, 0.f, 1.f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        }

        if( buffer )
        {
            glDrawPixels( _windowSize.x( ), _windowSize.y( ), format, type, buffer );
            if( _displayHelp )
            {
                KeyboardHandler& keyHandler = _brayns->getKeyboardHandler();
                _renderBitmapString( -0.98f, 0.95f, keyHandler.help() );
            }
        }
    }
    else
    {
        ScreenSpaceProcessorData ssProcData;

        ssProcData.width = _windowSize.x( );
        ssProcData.height = _windowSize.y( );

        ssProcData.colorFormat = GL_RGBA;
        ssProcData.colorBuffer = renderOutput.colorBuffer.data( );
        ssProcData.colorType = GL_UNSIGNED_BYTE;

        ssProcData.depthFormat = GL_LUMINANCE;
        ssProcData.depthBuffer = renderOutput.depthBuffer.data( );
        ssProcData.depthType = GL_FLOAT;

        _screenSpaceProcessor.draw( ssProcData );
    }

    float* buffer = renderOutput.depthBuffer.data();
    if( buffer )
    {
        size_t index = (_windowSize.y() - _mouse.y()) * _windowSize.x() + _mouse.x();
        _gid = buffer[index];
    }

    _fps.stop();

    glutSwapBuffers( );

    clearPixels( );

    const Vector2ui windowSize = _brayns->getParametersManager().getApplicationParameters().getWindowSize();
    if( windowSize != _windowSize )
        glutReshapeWindow(windowSize.x(), windowSize.y());
}

void BaseWindow::clearPixels( )
{
    _screenSpaceProcessor.clear();
}

void BaseWindow::drawPixels(const int* framebuffer)
{
    glDrawPixels(_windowSize.x( ), _windowSize.y( ),
                 GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
    glutSwapBuffers( );
}

void BaseWindow::drawPixels(const Vector3f* framebuffer)
{
    glDrawPixels(_windowSize.x( ), _windowSize.y( ),
                 GL_RGBA, GL_FLOAT, framebuffer);
    glutSwapBuffers( );
}

void BaseWindow::setTitle(const char *title)
{
    assert(_windowID>=0);
    glutSetWindow( _windowID );
    glutSetWindowTitle( title );
}

void BaseWindow::create(const char *title,
                        const size_t width, const size_t height,
                        bool fullScreen)
{
    glutInitWindowSize(width, height);
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

    if(fullScreen)
        glutFullScreen( );

    _screenSpaceProcessor.init( width, height );
}

void BaseWindow::keypress( const char key, const Vector2f& )
{
    switch( key )
    {
    case 'h':
        _displayHelp = !_displayHelp;
        break;
    default:
        _brayns->getKeyboardHandler().handleKeyboardShortcut( key );
    }

    _brayns->commit( );
}

void BaseWindow::specialkey( const int key, const Vector2f& )
{
    switch( key )
    {
    case GLUT_KEY_LEFT:
        _brayns->getKeyboardHandler().handle( SpecialKey::LEFT );
        break;
    case GLUT_KEY_RIGHT:
        _brayns->getKeyboardHandler().handle( SpecialKey::RIGHT );
        break;
    case GLUT_KEY_UP:
        _brayns->getKeyboardHandler().handle( SpecialKey::UP );
        break;
    case GLUT_KEY_DOWN:
        _brayns->getKeyboardHandler().handle( SpecialKey::DOWN );
        break;
    }
}

void BaseWindow::_registerKeyboardShortcuts()
{
    KeyboardHandler& keyHandler = _brayns->getKeyboardHandler();
    keyHandler.registerKeyboardShortcut(
        ' ', "Camera reset to initial state",
        std::bind( &BaseWindow::_resetCamera, this ));
    keyHandler.registerKeyboardShortcut(
        '+', "Increase motion speed",
        std::bind( &BaseWindow::_increaseMotionSpeed, this ));
    keyHandler.registerKeyboardShortcut(
        '-', "Decrease motion speed",
        std::bind( &BaseWindow::_decreaseMotionSpeed, this ));
    keyHandler.registerKeyboardShortcut(
        'c', "Display current camera information",
        std::bind( &BaseWindow::_displayCameraInformation, this ));
    keyHandler.registerKeyboardShortcut(
        'Q', "Quit application",
        std::bind( &BaseWindow::_exitApplication, this ));
    keyHandler.registerKeyboardShortcut(
        'z', "Switch between depth and color buffers",
        std::bind( &BaseWindow::_toggleFrameBuffer, this ));
}

void BaseWindow::_renderBitmapString( const float x, const float y,
                                      const std::string& text )
{
    glRasterPos3f( x, y, 0.f );
    glutBitmapString( GLUT_BITMAP_8_BY_13,
                      reinterpret_cast< const unsigned char* >( text.c_str( )));
    glRasterPos3f( -1.f, -1.f, 0.f );
}

void BaseWindow::_resetCamera()
{
    _brayns->getCamera().reset();
    _brayns->getCamera().commit();
}

void BaseWindow::_increaseMotionSpeed()
{
    _brayns->getCameraManipulator().updateMotionSpeed( DEFAULT_MOTION_ACCELERATION );
}

void BaseWindow::_decreaseMotionSpeed()
{
    _brayns->getCameraManipulator().updateMotionSpeed( 1.f / DEFAULT_MOTION_ACCELERATION );
}

void BaseWindow::_displayCameraInformation()
{
    BRAYNS_INFO << _brayns->getCamera() << std::endl;
}

void BaseWindow::_exitApplication()
{
#ifdef __APPLE__
    exit(0);
#else
    glutLeaveMainLoop();
#endif
}

void BaseWindow::_toggleFrameBuffer()
{
    if( _frameBufferMode == FrameBufferMode::DEPTH )
        _frameBufferMode = FrameBufferMode::COLOR;
    else
        _frameBufferMode = FrameBufferMode::DEPTH;
}

}
