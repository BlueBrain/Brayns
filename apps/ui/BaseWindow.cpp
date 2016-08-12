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
#include <brayns/common/camera/Camera.h>
#include <brayns/common/renderer/FrameBuffer.h>

#include <assert.h>

#ifdef __APPLE__
#  include "GLUT/glut.h"
#  include <unistd.h>
#else
#  include "GL/glut.h"
#endif

namespace brayns
{

const float DEFAULT_EPSILON = 1e-4f;
const float DEFAULT_MOTION_ACCELERATION = 1.5f;
const float DEFAULT_MOUSE_SPEED = 0.005f;

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

// ------------------------------------------------------------------
// Base window
// ------------------------------------------------------------------
/*! currently active window */
BaseWindow *BaseWindow::_activeWindow = nullptr;

BaseWindow::BaseWindow(
        BraynsPtr brayns, const int , const char **,
        const FrameBufferMode frameBufferMode,
        const ManipulatorMode initialManipulator,
        const int allowedManipulators)
  : _brayns(brayns), _lastMousePos(-1,-1), _currMousePos(-1,-1),
    _lastButtonState(0), _currButtonState(0),
    _currModifiers(0), _upVectorFromCmdLine(0,1,0),
    _motionSpeed(DEFAULT_MOUSE_SPEED), _rotateSpeed(DEFAULT_MOUSE_SPEED),
    _frameBufferMode(frameBufferMode),
    _windowID(-1), _windowSize(-1,-1), fullScreen_(false),
    frameCounter_(0)
{
    _setViewPort( );

    // Initialize manipulators
    if(allowedManipulators & INSPECT_CENTER_MODE)
        _inspectCenterManipulator.reset(new InspectCenterManipulator(*this));

    if(allowedManipulators & MOVE_MODE)
        _flyingModeManipulator.reset(new FlyingModeManipulator(*this));

    switch(initialManipulator)
    {
    case MOVE_MODE:
        _manipulator = _flyingModeManipulator.get( );
        break;
    case INSPECT_CENTER_MODE:
        _manipulator = _inspectCenterManipulator.get( );
        break;
    }
    assert(_manipulator);
}


BaseWindow::~BaseWindow( )
{
}

void BaseWindow::mouseButton(
        int whichButton,
        bool released,
        const Vector2i& pos)
{
    if(pos != _currMousePos)
        motion(pos);
    _lastButtonState = _currButtonState;

    if(released)
        _currButtonState = _currButtonState & ~(1<<whichButton);
    else
        _currButtonState = _currButtonState |  (1<<whichButton);
    _currModifiers = glutGetModifiers( );

    _manipulator->button( pos );
}

void BaseWindow::motion(const Vector2i& pos)
{
    _currMousePos = pos;
    if(_currButtonState != _lastButtonState)
    {
        // some button got pressed; reset 'old' pos to new pos.
        _lastMousePos = _currMousePos;
        _lastButtonState = _currButtonState;
    }

    _manipulator->motion( );
    _lastMousePos = _currMousePos;
}

void BaseWindow::idle( )
{
    usleep(1000);
}

void BaseWindow::reshape(const Vector2i& newSize)
{
    _windowSize = newSize;
    _viewPort.setAspect(float(newSize.x( ))/float(newSize.y( )));
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
    if(_viewPort.getModified( ))
    {
        _brayns->getFrameBuffer().clear();
        _viewPort.setModified(false);
    }

    _fps.start();

    RenderInput renderInput;
    RenderOutput renderOutput;

    renderInput.windowSize = _windowSize;
    renderInput.position = _viewPort.getPosition( );
    renderInput.target = _viewPort.getTarget( );
    renderInput.up = _viewPort.getUp( );

    _brayns->getCamera().commit();
    _brayns->render( renderInput, renderOutput );

    if( _brayns->getParametersManager().getApplicationParameters().getFilters().empty( ))
    {

        GLenum format = GL_RGBA;
        GLenum type   = GL_FLOAT;
        GLvoid* buffer = 0;
        switch(_frameBufferMode)
        {
        case BaseWindow::FRAMEBUFFER_COLOR:
            type = GL_UNSIGNED_BYTE;
            buffer = renderOutput.colorBuffer.data( );
            break;
        case BaseWindow::FRAMEBUFFER_DEPTH:
            format = GL_LUMINANCE;
            buffer = renderOutput.depthBuffer.data( );
            break;
        default:
            glClearColor(0.f,0.f,0.f,1.f);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        }

        glDrawPixels( _windowSize.x( ), _windowSize.y( ), format, type, buffer );
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

    _fps.stop();
    glutSwapBuffers( );

    clearPixels( );

#if(BRAYNS_USE_ZEROEQ || BRAYNS_USE_DEFLECT)
    Camera& camera = _brayns->getCamera( );
    if( camera.getPosition( ) != _viewPort.getPosition( ))
        _viewPort.setPosition( camera.getPosition( ) );
    if( camera.getTarget( ) != _viewPort.getTarget( ))
        _viewPort.setTarget( camera.getTarget( ) );
    if( camera.getUpVector( ) != _viewPort.getUp( ))
        _viewPort.setUp( camera.getUpVector( ) );
#endif

    const Vector2ui windowSize = _brayns->getParametersManager().getApplicationParameters().getWindowSize();
    if( windowSize != _windowSize )
        glutReshapeWindow(windowSize.x(), windowSize.y());
    ++frameCounter_;
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

Boxf BaseWindow::getWorldBounds( )
{
    return _brayns->getScene( ).getWorldBounds( );
}

void BaseWindow::_setViewPort( )
{
    Camera& camera = _brayns->getCamera();
    const Vector3f& position = camera.getPosition();
    const Vector3f& target = camera.getTarget();
    _viewPort.initialize( position, target, camera.getUpVector());

    _motionSpeed = Vector3f(target-position).length( ) * 0.001f;
    BRAYNS_INFO << "Viewport     :" << _viewPort << std::endl;
    BRAYNS_INFO << "Motion speed :" << _motionSpeed << std::endl;
    camera.commit();
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
    glutIdleFunc(glut3dIdle);

    if(fullScreen)
        glutFullScreen( );

    _screenSpaceProcessor.init( width, height );
}

void BaseWindow::specialkey( int key, const Vector2f& )
{
    if(_manipulator)
        _manipulator->specialkey( key );
}

void BaseWindow::keypress( char key, const Vector2f& )
{
    RenderingParameters& renderParams =
        _brayns->getParametersManager( ).getRenderingParameters( );
    SceneParameters& sceneParams =
        _brayns->getParametersManager().getSceneParameters();

    switch( key )
    {
    case ' ':
        BRAYNS_INFO << "Camera reset to initial state" << std::endl;
        _brayns->getCamera().reset();
        _brayns->getCamera().commit();
        break;
    case '+':
        _motionSpeed *= DEFAULT_MOTION_ACCELERATION;
        BRAYNS_INFO << "Motion speed: " << _motionSpeed << std::endl;
        break;
    case '-':
        _motionSpeed /= DEFAULT_MOTION_ACCELERATION;
        BRAYNS_INFO << "Motion speed: " << _motionSpeed << std::endl;
        break;
    case '1':
        renderParams.setBackgroundColor(Vector3f(.5f,.5f,.5f));
        BRAYNS_INFO << "Setting grey background" << std::endl;
        break;
    case '2':
        renderParams.setBackgroundColor(Vector3f(1.f,1.f,1.f));
        BRAYNS_INFO << "Setting white background" << std::endl;
        break;
    case '3':
        renderParams.setBackgroundColor(Vector3f(0.f,0.f,0.f));
        BRAYNS_INFO << "Setting black background" << std::endl;
        break;
    case 'C':
        BRAYNS_INFO << _viewPort << std::endl;
        break;
    case 'E':
        renderParams.setMaterialType( MT_ELECTRON );
        BRAYNS_INFO << "Electron shading activated" << std::endl;
        break;
    case 'F':
        // 'f'ly mode
        if( _flyingModeManipulator )
        {
            BRAYNS_INFO << "Switching to flying mode" << std::endl;
            _manipulator = _flyingModeManipulator.get();
        }
        break;
    case 'H':
        renderParams.setSoftShadows( !renderParams.getSoftShadows( ));
        BRAYNS_INFO << "Soft shadows " <<
            (renderParams.getSoftShadows( ) ? "On" : "Off") << std::endl;
        break;
    case 'I':
        // 'i'nspect mode
        if( _inspectCenterManipulator)
        {
            BRAYNS_INFO << "Switching to inspect mode" << std::endl;
            _manipulator = _inspectCenterManipulator.get();
        }
        break;
    case 'L':
    {
        fullScreen_ = !fullScreen_;
        if(fullScreen_)
            glutFullScreen( );
        else
            glutPositionWindow(0,10);
        break;
    }
    case 'o':
    {
        float aaStrength = _brayns->getParametersManager( ).
            getRenderingParameters( ).getAmbientOcclusionStrength( );
        aaStrength += 0.1f;
        if( aaStrength>1.f ) aaStrength=1.f;
        renderParams.setAmbientOcclusionStrength( aaStrength );
        BRAYNS_INFO << "Ambient occlusion strength: " <<
            aaStrength << std::endl;
        break;
    }
    case 'O':
    {
        float aaStrength = renderParams.getAmbientOcclusionStrength( );
        aaStrength -= 0.1f;
        if( aaStrength<0.f ) aaStrength=0.f;
        renderParams.setAmbientOcclusionStrength( aaStrength );
        BRAYNS_INFO << "Ambient occlusion strength: "
            << aaStrength << std::endl;
        break;
    }
    case 'p':
        renderParams.setMaterialType( MT_DIFFUSE );
        BRAYNS_INFO << "Diffuse shading activated" << std::endl;
        break;
    case 'P':
        renderParams.setMaterialType( MT_NO_SHADING );
        BRAYNS_INFO << "No shading activated" << std::endl;
        break;
    case 'r':
        sceneParams.setTimestamp( 0.f );
        BRAYNS_INFO << "Timestamp: " <<
            sceneParams.getTimestamp( ) << std::endl;
        break;
    case 'R':
        sceneParams.setTimestamp( std::numeric_limits< size_t >::max( ));
        BRAYNS_INFO << "Timestamp: " <<
            sceneParams.getTimestamp( ) << std::endl;
        break;
    case 'S':
        renderParams.setShadows(
            !renderParams.getShadows( ));
        BRAYNS_INFO << "Shadows: " <<
            (renderParams.getShadows( ) ? "On" : "Off") << std::endl;
        break;
    case 'V':
        renderParams.
            setBackgroundColor( Vector3f( rand( ) % 200 / 100.f - 1.f,
            rand( ) % 200 / 100.f - 1.f, rand( ) % 200 / 100.f - 1.f ));
        break;
    case 'Y':
        renderParams.setLightEmittingMaterials(
            !renderParams.getLightEmittingMaterials( ));
        break;
    case 'Z':
        if( _frameBufferMode==FRAMEBUFFER_DEPTH )
            _frameBufferMode = FRAMEBUFFER_COLOR;
        else
            _frameBufferMode = FRAMEBUFFER_DEPTH;
        break;
    }
    if(_manipulator)
        _manipulator->keypress( key );
}

}
