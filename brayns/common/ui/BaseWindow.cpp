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

#ifndef __APPLE__
// GCC automtically removes the library if the application does not
// make an explicit use of one of its classes. In the case of OSPRay
// classes are loaded dynamicaly. The following line is only to make
// sure that the hbpKernel library is loaded.
#  include <brayns/kernels/render/ExtendedOBJRenderer.h>
brayns::ExtendedOBJRenderer __r__;
#endif

#include "BaseWindow.h"
#include <brayns/common/log.h>
#include <brayns/common/algorithms/MetaballsGenerator.h>

#ifdef __APPLE__
#  include "GLUT/glut.h"
#  include <unistd.h>
#else
#  include "GL/glut.h"
#endif

#ifdef BRAYNS_USE_DEFLECT
#  include "../extensions/DeflectManager.h"
#endif

#ifdef BRAYNS_USE_RESTBRIDGE
#  include <turbojpeg.h>
#  include <servus/uri.h>
#  include <boost/bind.hpp>
#endif

#ifdef BRAYNS_USE_ASSIMP
#  include <brayns/common/loaders/MeshLoader.h>
#endif

namespace brayns
{

const float DEFAULT_GAMMA   = 2.2f;
const float DEFAULT_EPSILON = 1e-4f;
const float DEFAULT_MOTION_ACCELERATION = 1.5f;
const float DEFAULT_MOUSE_SPEED = 1e-3f;

void runGLUT()
{
    glutMainLoop();
}

void initGLUT(int32 *ac, const char **av)
{
    glutInit(ac, (char **) av);
}

// ------------------------------------------------------------------
// glut event handlers
// ------------------------------------------------------------------
void glut3dReshape(int32 x, int32 y)
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->reshape(ospray::vec2i(x,y));
}

void glut3dDisplay( void )
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->display();
}

void glut3dKeyboard(unsigned char key, int32 x, int32 y)
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->keypress(key,ospray::vec2i(x,y));
}
void glut3dSpecial(int32 key, int32 x, int32 y)
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->specialkey(key,ospray::vec2i(x,y));
}

void glut3dIdle( void )
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->idle();
}
void glut3dMotionFunc(int32 x, int32 y)
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->motion(ospray::vec2i(x,y));
}

void glut3dMouseFunc(int32 whichButton, int32 released, int32 x, int32 y)
{
    if (BaseWindow::activeWindow_)
        BaseWindow::activeWindow_->mouseButton(
                    whichButton,released,ospray::vec2i(x,y));
}

// ------------------------------------------------------------------
// Base window
// ------------------------------------------------------------------
/*! currently active window */
BaseWindow *BaseWindow::activeWindow_ = NULL;

BaseWindow::BaseWindow(
        const ApplicationParameters& applicationParameters,
        const FrameBufferMode frameBufferMode,
        const ManipulatorMode initialManipulator,
        const int allowedManipulators)
  : lastMousePos_(-1,-1), currMousePos_(-1,-1),
    lastButtonState_(0), currButtonState_(0),
    currModifiers_(0), upVectorFromCmdLine_(0,1,0),
    motionSpeed_(DEFAULT_MOUSE_SPEED), rotateSpeed_(DEFAULT_MOUSE_SPEED),
    ucharFB_(NULL), frameBufferMode_(frameBufferMode),
    applicationParameters_(applicationParameters),
    windowID_(-1), windowSize_(-1,-1), fullScreen_(false),
    fb_(NULL), renderer_(NULL), camera_(NULL), model_(NULL),
    frameNumber_(inf), frameCounter_(0),
    metaballsGridDimension_(0)
    #ifdef BRAYNS_USE_RESTBRIDGE
    , handleCompress_( tjInitCompress() )
    #else
    , handleCompress_( 0 )
    #endif
{
    worldBounds_.lower = ospray::vec3f(-1);
    worldBounds_.upper = ospray::vec3f(+1);

    if (allowedManipulators & INSPECT_CENTER_MODE)
        inspectCenterManipulator = new InspectCenterManipulator(this);

    if (allowedManipulators & MOVE_MODE)
        flyingModeManipulator = new FlyingModeManipulator(this);

    switch(initialManipulator)
    {
    case MOVE_MODE:
        manipulator = flyingModeManipulator;
        break;
    case INSPECT_CENTER_MODE:
        manipulator = inspectCenterManipulator;
        break;
    }
    Assert2(manipulator != NULL,"invalid initial manipulator mode");
}

BaseWindow::~BaseWindow()
{
#ifdef BRAYNS_USE_RESTBRIDGE
    if( handleCompress_ )
        tjDestroy(handleCompress_);
#endif
}

void BaseWindow::mouseButton(
        int32 whichButton,
        bool released,
        const ospray::vec2i& pos)
{

    if (pos != currMousePos_)
        motion(pos);
    lastButtonState_ = currButtonState_;

    if (released)
        currButtonState_ = currButtonState_ & ~(1<<whichButton);
    else
        currButtonState_ = currButtonState_ |  (1<<whichButton);
    currModifiers_ = glutGetModifiers();

    manipulator->button( pos );
}

void BaseWindow::motion(const ospray::vec2i& pos)
{
    currMousePos_ = pos;
    if(currButtonState_ != lastButtonState_)
    {
        // some button got pressed; reset 'old' pos to new pos.
        lastMousePos_ = currMousePos_;
        lastButtonState_ = currButtonState_;
    }

    manipulator->motion();
    lastMousePos_ = currMousePos_;
    if (viewPort_.modified)
        forceRedraw();
}

void BaseWindow::computeFrame()
{
    viewPort_.frame.l.vy = normalize(viewPort_.at - viewPort_.from);
    viewPort_.frame.l.vx = normalize(
                cross(viewPort_.frame.l.vy,viewPort_.up));
    viewPort_.frame.l.vz = normalize(
                cross(viewPort_.frame.l.vx,viewPort_.frame.l.vy));
    viewPort_.frame.p    = viewPort_.from;
    viewPort_.snapUp();
    viewPort_.modified = true;
}

void BaseWindow::setZUp(const ospray::vec3f &up)
{
    viewPort_.up = up;
    if (up != ospray::vec3f(0.f)) {
        viewPort_.snapUp();
        forceRedraw();
    }
}

void BaseWindow::idle()
{
    usleep(1000);
}

void BaseWindow::reshape(const ospray::vec2i &newSize)
{
    windowSize_ = newSize;
    viewPort_.aspect = newSize.x/float(newSize.y);
    if (fb_) ospFreeFrameBuffer(fb_);
    fb_ = ospNewFrameBuffer(
                newSize, OSP_RGBA_I8, OSP_FB_COLOR|OSP_FB_DEPTH|OSP_FB_ACCUM );
    ospSet1f(fb_, "gamma", DEFAULT_GAMMA);
    ospCommit(fb_);
    ospFrameBufferClear(fb_,OSP_FB_ACCUM);
    ospSetf(camera_,"aspect",viewPort_.aspect);
    ospCommit(camera_);
    viewPort_.modified = true;
    forceRedraw();
}

void BaseWindow::activate()
{
    activeWindow_ = this;
    glutSetWindow(windowID_);
}

void BaseWindow::forceRedraw()
{
    glutPostRedisplay();
}

void BaseWindow::setRendererParameters()
{
    ospSet1i(renderer_, "shadowsEnabled",
             renderingParameters_.getShadows());
    ospSet1i(renderer_, "softShadowsEnabled",
             renderingParameters_.getSoftShadows());
    ospSet1i(renderer_, "ambientOcclusionEnabled",
             renderingParameters_.getAmbientOcclusion());
    ospSet1i(renderer_, "shadingEnabled",
             renderingParameters_.getLightShading());
    ospSet1i(renderer_, "frameNumber",
             frameNumber_);
    ospSet1i(renderer_, "randomNumber",
             rand()%1000);
    //ospSet1i(renderer_, "moving", viewPort_.modified);
    ospSet1i(renderer_, "spp",
             renderingParameters_.getSamplesPerPixel());
    ospSet1i(renderer_, "electronShading",
             renderingParameters_.getElectronShading());
    ospSet1i(renderer_, "lightEmittingMaterialsEnabled",
             renderingParameters_.getLightEmittingMaterials());
    ospSet1i(renderer_, "gradientBackgroundEnabled",
             renderingParameters_.getGradientBackground());
    ospSet1f(renderer_, "epsilon", DEFAULT_EPSILON);

    if( renderingParameters_.getDepthOfField() != 0.f )
    {
        ospray::vec3f pos = viewPort_.from;
        float dofStrength = renderingParameters_.getDepthOfFieldStrength();
        pos.x += dofStrength*(rand()%100-50);
        pos.y += dofStrength*(rand()%100-50);
        pos.z += dofStrength*(rand()%100-50);
        ospSetVec3f(camera_,"dir",viewPort_.at-pos);
        ospSetVec3f(camera_,"pos",pos);
        ospCommit(camera_);
    }

    if ( viewPort_.modified)
    {
        ospSet1i(renderer_, "shadingEnabled", true);
        Assert2(camera_,"ospshadowRayray camera is null");
        ospSetVec3f(camera_,"pos",viewPort_.from);
        ospSetVec3f(camera_,"dir",viewPort_.at-viewPort_.from);
        ospSetVec3f(camera_,"up",viewPort_.up);
        ospSetf(camera_,"aspect",viewPort_.aspect);
        ospCommit(camera_);
        viewPort_.modified = false;
        ospFrameBufferClear(fb_,OSP_FB_ACCUM);
    }

    ospCommit(renderer_);
}

void BaseWindow::display()
{
#ifdef BRAYNS_USE_DEFLECT
    initializeDeflect();
#endif

#ifdef BRAYNS_USE_RESTBRIDGE
    initializeRest();
#endif

    setRendererParameters();

    fps_.startRender();
    ospRenderFrame(fb_,renderer_,OSP_FB_COLOR|OSP_FB_ACCUM);
    fps_.doneRender();

    GLenum format = GL_RGBA;
    GLenum type   = GL_FLOAT;
    GLvoid* buffer = 0;
    if (frameBufferMode_ == BaseWindow::FRAMEBUFFER_UCHAR && ucharFB_)
    {
        type = GL_UNSIGNED_BYTE;
        buffer = ucharFB_;
    }
    else if (frameBufferMode_ == BaseWindow::FRAMEBUFFER_FLOAT && floatFB_)
    {
        buffer = floatFB_;
    }
    else if (frameBufferMode_ == BaseWindow::FRAMEBUFFER_DEPTH && depthFB_)
    {
        format = GL_LUMINANCE;
        buffer = depthFB_;
    }
    else
    {
        glClearColor(0.f,0.f,0.f,1.f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
    glDrawPixels(windowSize_.x, windowSize_.y, format, type, buffer);
    glutSwapBuffers();

#ifdef BRAYNS_USE_DEFLECT
    if( deflectManager_)
        deflectManager_->send(windowSize_, ucharFB_, true);
#endif
    ++frameCounter_;
}

void BaseWindow::saveFrameToDisk(
        const size_t frameIndex,
        const std::string& prefix )
{
    char tmpFileName[] = "frame";
    static const char *dumpFileRoot;
    if (!dumpFileRoot)
        dumpFileRoot = getenv("OSPRAY_SCREEN_DUMP_ROOT");

    if (!dumpFileRoot)
    {
        if( mkstemp(tmpFileName) )
            dumpFileRoot = tmpFileName;
        else
        {
            BRAYNS_ERROR << "Failed to generated filename" << std::endl;
            return;
        }
    }

    char fileName[2048];
    std::string format = prefix + "_%s_%08d.ppm";
    sprintf(fileName, format.c_str(), dumpFileRoot,
            static_cast<int>(frameIndex));

    FILE *file = fopen(fileName,"wb");
    if (!file)
    {
        BRAYNS_ERROR << "Could not create screen shot file '"
                     << fileName << "'" << std::endl;
        return;
    }

    // Write file header
    fprintf(file,"P6\n%i %i\n255\n",windowSize_.x,windowSize_.y);

    // Write file contents
    std::vector<unsigned char> out;
    for (int y=0; y<windowSize_.y; ++y)
    {
        const unsigned char *in =
                (const unsigned char *)&ucharFB_[
                (windowSize_.y-1-y)*windowSize_.x];

        for (int x=0; x<windowSize_.x; ++x)
        {
            out.push_back(in[4*x+0]);
            out.push_back(in[4*x+1]);
            out.push_back(in[4*x+2]);
        }
        fwrite(out.data(),3*windowSize_.x,sizeof(char),file);
        out.clear();
    }
    fprintf(file,"\n");
    fclose(file);

    BRAYNS_INFO << "Saved framebuffer to file " << fileName << std::endl;
}

void BaseWindow::clearPixels()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void BaseWindow::drawPixels(const uint32 *framebuffer)
{
    glDrawPixels( windowSize_.x, windowSize_.y,
                  GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
    glutSwapBuffers();
}

void BaseWindow::drawPixels(const ospray::vec3fa *framebuffer)
{
    glDrawPixels(windowSize_.x, windowSize_.y,
                 GL_RGBA, GL_FLOAT, framebuffer);
    glutSwapBuffers();
}

void BaseWindow::setViewPort(const ospray::vec3f from,
                             const ospray::vec3f at,
                             const ospray::vec3f up)
{
    const ospray::vec3f dir = at - from;
    viewPort_.at    = at;
    viewPort_.from  = from;
    viewPort_.up    = up;

    this->worldBounds_ = worldBounds_;
    viewPort_.frame.l.vy = normalize(dir);
    viewPort_.frame.l.vx = normalize(
                cross(viewPort_.frame.l.vy,up));
    viewPort_.frame.l.vz = normalize(
                cross(viewPort_.frame.l.vx,viewPort_.frame.l.vy));
    viewPort_.frame.p    = from;
    viewPort_.snapUp();
    viewPort_.modified = true;
}

void BaseWindow::setWorldBounds(const ospray::box3f &worldBounds)
{
    ospray::vec3f center = embree::center(worldBounds);
    ospray::vec3f diag   = worldBounds.size();
    diag         = max(diag,ospray::vec3f(0.3f*length(diag)));
    ospray::vec3f from   = center;
    from.z -= diag.z;
    ospray::vec3f dir = center - from;
    ospray::vec3f up  = viewPort_.up;

    viewPort_.at    = center;
    viewPort_.from  = from;
    viewPort_.up    = up;

    if (length(up) < DEFAULT_EPSILON)
        up = ospray::vec3f(0,0,1.f);

    this->worldBounds_ = worldBounds_;
    viewPort_.frame.l.vy = normalize(dir);
    viewPort_.frame.l.vx = normalize(
                cross(viewPort_.frame.l.vy,up));
    viewPort_.frame.l.vz = normalize(
                cross(viewPort_.frame.l.vx,viewPort_.frame.l.vy));
    viewPort_.frame.p    = from;
    viewPort_.snapUp();
    viewPort_.modified = true;

    motionSpeed_ = length(diag) * DEFAULT_EPSILON;
    BRAYNS_INFO << "World bounds " << worldBounds_ <<
                   ", motion speed " << motionSpeed_ << std::endl;
}

void BaseWindow::setTitle(const char *title)
{
    Assert2(windowID_ >= 0,
            "must call BaseWindow::create() before calling setTitle()");
    glutSetWindow( windowID_ );
    glutSetWindowTitle( title );
}

void BaseWindow::create(const char *title,
                        const size_t width, const size_t height,
                        bool fullScreen)
{
    glutInitWindowSize( width, height );
    windowID_ = glutCreateWindow( title );
    activeWindow_ = this;
    glutDisplayFunc( glut3dDisplay );
    glutReshapeFunc( glut3dReshape );
    glutKeyboardFunc(glut3dKeyboard );
    glutSpecialFunc( glut3dSpecial );
    glutMotionFunc(  glut3dMotionFunc );
    glutMouseFunc(   glut3dMouseFunc );
    glutIdleFunc(    glut3dIdle );

    if (fullScreen)
        glutFullScreen();
}

void BaseWindow::specialkey( int32 key, const ospray::vec2f )
{
    if (manipulator)
        manipulator->specialkey( key );
}

void BaseWindow::keypress( char key, const ospray::vec2f )
{
    switch( key )
    {
    case '+':
        motionSpeed_ *= DEFAULT_MOTION_ACCELERATION;
        break;
    case '-':
        motionSpeed_ /= DEFAULT_MOTION_ACCELERATION;
        break;
    case 'B':
        ospSet3f(renderer_, "bgColor", 1.0f, 1.0f, 1.0f);
        break;
    case 'b':
        ospSet3f(renderer_, "bgColor", 0.f, 0.f, 0.f);
        ospCommit(renderer_);
        break;
    case 'C':
        BRAYNS_INFO << viewPort_ << std::endl;
        break;
    case 'D':
        renderingParameters_.setDepthOfField(
                    !renderingParameters_.getDepthOfField());
        break;
    case 'E':
        renderingParameters_.setElectronShading(
                    !renderingParameters_.getElectronShading());
        break;
    case 'F':
        // 'f'ly mode
        if( flyingModeManipulator )
            manipulator = flyingModeManipulator;
        break;
    case 'G':
        renderingParameters_.setGradientBackground(
                    !renderingParameters_.getGradientBackground());
        break;
    case 'H':
        renderingParameters_.setSoftShadows(
                    !renderingParameters_.getSoftShadows());
        break;
    case 'I':
        // 'i'nspect mode
        if( inspectCenterManipulator)
            manipulator = inspectCenterManipulator;
        break;
    case 'L':
    {
        fullScreen_ = !fullScreen_;
        if(fullScreen_)
            glutFullScreen();
        else
            glutPositionWindow(0,10);
    }
        break;
    case 'W':
        for (size_t i=0;i<materials_.size();++i)
        {
            float a = float(rand()%255/255.0);
            ospSet3f(materials_[i], "kd", a, a, a);
            ospCommit(materials_[i]);
        }
        ospFrameBufferClear(fb_,OSP_FB_ACCUM);
        break;
    case 'M':
        for (size_t i=0;i<materials_.size();++i)
        {
            float d;
            ospGetf(materials_[i], "d", &d);
            if( d==1.f )
            {
                // Reflector
                ospSet1f(materials_[i], "d", -0.7);
                ospSet3f(materials_[i], "ks", 0.1, 0.1, 0.1);
                ospSet1f(materials_[i], "ns", 5);
            }
            ospCommit(materials_[i]);
        }
        ospFrameBufferClear(fb_,OSP_FB_ACCUM);
        break;
    case 'm':
        for (size_t  i=0;i<materials_.size();++i)
        {
            ospSet3f(materials_[i], "kd",
                     (rand()%255/255.0),
                     (rand()%255/255.0),
                     (rand()%255/255.0));
            if( i==materials_.size()-1 )
                // last material is set to black
                ospSet3f(materials_[i], "kd", 0, 0, 0);

            ospSet1f(materials_[i], "d", 1.0);
            ospSet1f(materials_[i], "a", 0.0);
            switch( rand()%4 )
            {
            case 0:
                // Transparent
                ospSet1f(materials_[i], "d", 0.1);
                ospSet1f(materials_[i], "r", 0.98);
                ospSet3f(materials_[i], "ks", 0.01, 0.01, 0.01);
                ospSet1f(materials_[i], "ns", 10);
                break;
            case 1:
                // Light emmitter
                ospSet1f(materials_[i], "a", 5.0);
                break;
            case 2:
                // Reflector
                ospSet1f(materials_[i], "d", -0.7);
                ospSet3f(materials_[i], "ks", 0.01, 0.01, 0.01);
                ospSet1f(materials_[i], "ns", 10);
                break;
            }
            ospCommit(materials_[i]);
        }
        ospFrameBufferClear(fb_,OSP_FB_ACCUM);
        break;
    case 'O':
        renderingParameters_.setAmbientOcclusion(
                    !renderingParameters_.getAmbientOcclusion());
        break;
    case 'P':
        renderingParameters_.setLightShading(
                    !renderingParameters_.getLightShading());
        break;
    case 'r':
        frameNumber_ = 0;
        break;
    case 'R':
        frameNumber_ = inf;
        break;
    case 'S':
        renderingParameters_.setShadows(!renderingParameters_.getShadows());
        break;
    case 'V':
        ospSet3f(renderer_, "bgColor", rand()%200/100.f-1.f,
                 rand()%200/100.f-1.f, rand()%200/100.0-1.f);

        ospCommit(renderer_);
        break;
    case 'v':
        lightDirection_.x = rand()%100/100.f,
        lightDirection_.y = -rand()%100/100.f;
        lightDirection_.z = rand()%100/100.f;
        ospSet3f(light_,"direction",
                 lightDirection_.x, lightDirection_.y, lightDirection_.z );
        ospCommit(light_);
        break;
    case 'Y':
        renderingParameters_.setLightEmittingMaterials(
                    !renderingParameters_.getLightEmittingMaterials());
        break;
    }
    if (manipulator)
        manipulator->keypress( key );
}

void BaseWindow::resetBounds()
{
    bounds_ = embree::empty;
}

ospray::box3f BaseWindow::getBounds() const
{
    return bounds_;
}

void BaseWindow::saveSceneToBinaryFile( const std::string& fn )
{
    BRAYNS_INFO << "Saving binary scene to " << fn << std::endl;
    FILE* file = fopen(fn.c_str(),"wb");
    size_t nb = materials_.size();
    fwrite(&nb, sizeof(size_t), 1, file);
    for( size_t i=0; i<nb; ++i )
    {
        size_t len = spheres_[i].size();
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(&spheres_[i][0], sizeof(Sphere), len, file );
    }
    fwrite(&bounds_, sizeof(ospray::box3f), 1, file );
    fclose(file);
}

void BaseWindow::loadSceneFromBinaryFile( const std::string& fn )
{
    FILE* file = fopen(fn.c_str(),"rb");
    if( file )
    {
        size_t nb;
        size_t status = fread(&nb, 1, sizeof(size_t), file);
        for( size_t i=0; status==0 && i<nb; ++i )
        {
            size_t s;
            status = fread(&s, sizeof(size_t), 1, file);
            BRAYNS_INFO << "[" << i << "] " << s << " spheres" << std::endl;

            spheres_[i].reserve(s);
            for( size_t n=0; n<s; ++n )
            {
                Sphere sphere;
                status = fread(&sphere, 1, sizeof(Sphere), file);
                spheres_[i].push_back(sphere);
            }
        }
        status = fread(&bounds_, sizeof(ospray::box3f), 1, file );
        fclose(file);
    }
}

#ifdef BRAYNS_USE_DEFLECT
void BaseWindow::initializeDeflect()
{
    if( !deflectManager_ && applicationParameters_.getDeflectHostname() != "" )
    {
        deflectManager_.reset(new DeflectManager(
                    applicationParameters_.getDeflectHostname(),
                    applicationParameters_.getDeflectStreamname(),
                    true, 100 ));
    }

    if( deflectManager_)
        deflectManager_->handleTouchEvents(this);
}
#endif

#ifdef BRAYNS_USE_RESTBRIDGE
void BaseWindow::initializeRest()
{
    if(!rcSubscriber_)
    {
        rcSubscriber_.reset(new zeq::Subscriber( servus::URI(
                        applicationParameters_.getZeqSchema() + "cmd://" )));
        rcSubscriber_->registerHandler( zeq::hbp::EVENT_CAMERA,
                                        boost::bind( &BaseWindow::onCamera,
                                                     this, _1 ));
        rcSubscriber_->registerHandler( zeq::vocabulary::EVENT_REQUEST,
                                        boost::bind( &BaseWindow::onRequest,
                                                     this, _1 ));
    }
    if(!rcPublisher_)
        rcPublisher_.reset(new zeq::Publisher( servus::URI(
                       applicationParameters_.getZeqSchema() + "resp://" )));

    while(rcSubscriber_->receive( 0 ));
    rcPublisher_->publish( zeq::Event( zeq::vocabulary::EVENT_HEARTBEAT ));

    // The RESTBridge should only be created once the zeq layer is fully
    // initialized
    if( !restBridge_ )
    {
        restBridge_.reset(new restbridge::RestBridge(
                    applicationParameters_.getRESTHostname(),
                    applicationParameters_.getRESTPort()));

        restBridge_->run( applicationParameters_.getZeqSchema() );
        BRAYNS_INFO << "Initializing restBridge " <<
                       applicationParameters_.getZeqSchema() <<
                       "://" << applicationParameters_.getRESTHostname() <<
                       ":" << applicationParameters_.getRESTPort() <<
                       std::endl;
    }
}

void BaseWindow::onCamera( const zeq::Event& event )
{
    if( event.getType() != zeq::hbp::EVENT_CAMERA ) return;
    const std::vector< float >& matrix = zeq::hbp::deserializeCamera(event);
    viewPort_.from = ospray::vec3f(matrix[0], matrix[1], matrix[2]);
    viewPort_.from = viewPort_.from * bounds_.size();

    ospSetVec3f(camera_, "pos", viewPort_.from);
    ospSetVec3f(camera_, "dir", viewPort_.at - viewPort_.from);
    ospCommit(camera_);
    ospFrameBufferClear(fb_,OSP_FB_ACCUM);
}

void BaseWindow::onRequest( const zeq::Event& event )
{
    const zeq::uint128_t& eventType =
            zeq::vocabulary::deserializeRequest( event );
    if( eventType == zeq::hbp::EVENT_IMAGEJPEG )
    {
        unsigned int* colorBuffer = (unsigned int*)ucharFB_;

        size_t nBytes = sizeof(colorBuffer) * 4;
        uint8_t* fb = new uint8_t[nBytes];
        uint8_t* fbPtr = (uint8_t*) &colorBuffer;
        for(size_t i=0; i<nBytes; i++)
        {
            fb[i] = *fbPtr;
            fbPtr++;
        }
        unsigned long jpegSize =
                windowSize_.x*windowSize_.y*sizeof(uint32);
        uint8_t* jpegData = _encodeJpeg(
                    (uint32_t)windowSize_.x, (uint32_t)windowSize_.y,
                    (uint8_t*)colorBuffer, jpegSize);

        const zeq::hbp::data::ImageJPEG image( jpegSize, jpegData );
        const zeq::Event& image_event = zeq::hbp::serializeImageJPEG( image );
        rcPublisher_->publish( image_event );
        delete [] fb;
    }
    else if( eventType == zeq::vocabulary::EVENT_VOCABULARY )
    {
        BRAYNS_INFO << "Registering application vocabulary" << std::endl;
        zeq::EventDescriptors vocabulary;
        vocabulary.push_back( zeq::EventDescriptor(
                                  zeq::hbp::IMAGEJPEG,
                                  zeq::hbp::EVENT_IMAGEJPEG,
                                  zeq::hbp::SCHEMA_IMAGEJPEG,
                                  zeq::PUBLISHER ) );
        vocabulary.push_back( zeq::EventDescriptor(
                                  zeq::hbp::CAMERA,
                                  zeq::hbp::EVENT_CAMERA,
                                  zeq::hbp::SCHEMA_CAMERA,
                                  zeq::SUBSCRIBER ) );
        vocabulary.push_back( zeq::EventDescriptor(
                                  zeq::hbp::CAMERA,
                                  zeq::hbp::EVENT_CAMERA,
                                  zeq::hbp::SCHEMA_CAMERA,
                                  zeq::PUBLISHER ) );

        const zeq::Event& vocEvent =
                zeq::vocabulary::serializeVocabulary( vocabulary );

        rcPublisher_->publish( vocEvent );
    }
    else if( eventType == zeq::hbp::EVENT_CAMERA )
    {
        std::vector<float> matrix;
        vec3f pos = viewPort_.from / bounds_.size();
        vec3f target = viewPort_.at / bounds_.size();
        matrix.push_back(-pos.x);
        matrix.push_back(pos.y);
        matrix.push_back(-pos.z);
        matrix.push_back(-target.x);
        matrix.push_back(target.y);
        matrix.push_back(-target.z);
        for( int i(0); i<10; ++i) matrix.push_back(0);
        const zeq::Event& camera_event = zeq::hbp::serializeCamera( matrix );
        rcPublisher_->publish( camera_event );
    }
}

uint8_t* BaseWindow::_encodeJpeg( const uint32_t width,
                                  const uint32_t height,
                                  const uint8_t* rawData,
                                  unsigned long& dataSize )
{
    uint8_t* tjSrcBuffer = const_cast< uint8_t* >(rawData);
    const int32_t pixelFormat = TJPF_RGBA;
    const int32_t color_components = 4; // Color Depth
    const int32_t tjPitch = width * color_components;
    const int32_t tjPixelFormat = pixelFormat;

    uint8_t* tjJpegBuf = 0;
    const int32_t tjJpegSubsamp = TJSAMP_444;
    const int32_t tjJpegQual = 100; // Image Quality
    const int32_t tjFlags = TJXOP_ROT180;

    const int32_t success =
            tjCompress2( handleCompress_, tjSrcBuffer, width, tjPitch, height,
                         tjPixelFormat, &tjJpegBuf, &dataSize, tjJpegSubsamp,
                         tjJpegQual, tjFlags);

    if(success != 0)
    {
        BRAYNS_INFO << "libjpeg-turbo image conversion failure" << std::endl;
        return 0;
    }
    return static_cast<uint8_t *>(tjJpegBuf);
}
#endif

void BaseWindow::generateMetaballs( const float threshold )
{
    // Metaballs
    MetaballSpheres metaballs;
    for( size_t i=0; i<materials_.size(); ++i )
    {
        brayns::Spheres::iterator it = spheres_[i].begin();
        while( it<spheres_[i].end() )
        {
            MetaballSphere metaball;
            metaball.sphere = (*it);
            metaball.sphere.r *= 10;
            metaball.materialId = i;
            metaballs.push_back(metaball);
            ++it;
        }
    }

    MetaballsGenerator mg(bounds_, metaballs, triangles_,
                          metaballsGridDimension_, threshold);

    size_t nbTriangles(0);
    for( size_t i=0; i<materials_.size(); ++i )
    {
        if( triangles_[i].vertex.size() != 0)
        {
            nbTriangles += triangles_[i].vertex.size();
            ospTriangles_[i] = ospNewTriangleMesh();
            BRAYNS_INFO << "Triangles[" << i
                        << "] vertices =" << triangles_[i].vertex.size()
                        << ", normals =" << triangles_[i].normal.size()
                        << ", indices =" << triangles_[i].index.size()
                        << std::endl;

            ospVertices_[i] = ospNewData(
                        triangles_[i].vertex.size(),OSP_FLOAT3A,
                        &triangles_[i].vertex[0],OSP_DATA_SHARED_BUFFER);
            ospSetObject(ospTriangles_[i],"vertex",ospVertices_[i]);

            ospIndices_[i] = ospNewData(
                        triangles_[i].index.size(),OSP_INT3,
                        &triangles_[i].index[0],OSP_DATA_SHARED_BUFFER);
            ospSetObject(ospTriangles_[i],"index",ospIndices_[i]);

            if( triangles_[i].normal.size()>0)
            {
                ospNormals_[i] = ospNewData(
                            triangles_[i].normal.size(),OSP_FLOAT3A,
                            &triangles_[i].normal[0],OSP_DATA_SHARED_BUFFER);
                ospSetObject(ospTriangles_[i],"vertex.normal",ospNormals_[i]);
            }

            if( true && triangles_[i].color.size()>0)
            {
                ospColors_[i] = ospNewData(
                            triangles_[i].color.size(),OSP_FLOAT4,
                            &triangles_[i].color[0],OSP_DATA_SHARED_BUFFER);
                ospSetObject(ospTriangles_[i],"vertex.color",ospColors_[i]);
            }

            if (materials_[i])
                ospSetMaterial(ospTriangles_[i], materials_[i]);
            ospCommit(ospTriangles_[i]);
            ospAddGeometry(model_, ospTriangles_[i]);
        }
    }
    BRAYNS_INFO << "Number of generated faces: " << nbTriangles << std::endl;
}

// ------------------------------------------------------------------
// Window viewport
// ------------------------------------------------------------------
BaseWindow::ViewPort::ViewPort()
    : modified(true),
      from(0,0,-1),
      up(0,1,0),
      at(0,0,0),
      openingAngle(60.f*M_PI/360.f),
      aspect(1.f)
{
    frame = AffineSpace3fa::translate(from) * AffineSpace3fa(embree::one);
}

void BaseWindow::ViewPort::snapUp()
{
    if (fabsf(dot(up,frame.l.vz)) < 1e-3f)
        return;
    frame.l.vx = normalize(cross(frame.l.vy,up));
    frame.l.vz = normalize(cross(frame.l.vx,frame.l.vy));
    frame.l.vy = normalize(cross(frame.l.vz,frame.l.vx));
}

}
