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
#include <signal.h>
#include <brayns/common/log.h>
#include <brayns/common/algorithms/MetaballsGenerator.h>
#include <brayns/common/extensions/ExtensionController.h>

#ifdef __APPLE__
#  include "GLUT/glut.h"
#  include <unistd.h>
#else
#  include "GL/glut.h"
#endif

#ifdef BRAYNS_USE_ASSIMP
#  include <brayns/common/loaders/MeshLoader.h>
#endif

namespace brayns
{

const float DEFAULT_GAMMA   = 2.2f;
const float DEFAULT_EPSILON = 1e-1f;
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
                    whichButton, released, ospray::vec2i(x,y));
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
    metaballsGridDimension_(0), running_(false)
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
    Assert2( manipulator != NULL, "invalid initial manipulator mode ");

    // Application is now running
    running_ = true;
}


BaseWindow::~BaseWindow()
{
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

void BaseWindow::setRendererParameters_()
{
    ospSet1i(renderer_, "shadowsEnabled",
             renderingParameters_.getShadows());
    ospSet1i(renderer_, "softShadowsEnabled",
             renderingParameters_.getSoftShadows());
    ospSet1f(renderer_, "ambientOcclusionStrength",
             renderingParameters_.getAmbientOcclusionStrength());
    ospSet1i(renderer_, "shadingEnabled",
             renderingParameters_.getLightShading());
    ospSet1i(renderer_, "frameNumber",
             frameNumber_);
    ospSet1i(renderer_, "randomNumber",
             rand()%1000);
    ospSet1i(renderer_, "moving", false /*viewPort_.modified*/);
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

    if( viewPort_.modified )
    {
        Assert2(camera_,"ospray camera is null");
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
    setRendererParameters_();

    if( !extensionController_ )
    {
        ExtensionParameters extensionParameters = {
            renderer_, camera_, ucharFB_, fb_, windowSize_, bounds_, running_ };
        extensionController_.reset(new ExtensionController(
            applicationParameters_, extensionParameters ));
    }
    extensionController_->execute();

    if( applicationParameters_.isBenchmarking() ) fps_.startRender();

    GLenum format = GL_RGBA;
    GLenum type   = GL_FLOAT;
    GLvoid* buffer = 0;
    switch( frameBufferMode_ )
    {
    case BaseWindow::FRAMEBUFFER_UCHAR:
        {
            ospRenderFrame(fb_,renderer_,OSP_FB_COLOR|OSP_FB_ACCUM|OSP_FB_ALPHA);
            type = GL_UNSIGNED_BYTE;
            ucharFB_ = (uint32 *)ospMapFrameBuffer(fb_, OSP_FB_COLOR);
            buffer = ucharFB_;
            break;
        }
    case BaseWindow::FRAMEBUFFER_FLOAT:
        {
            ospRenderFrame(fb_,renderer_,OSP_FB_COLOR|OSP_FB_ACCUM|OSP_FB_ALPHA);
            floatFB_ = (vec3fa *)ospMapFrameBuffer(fb_, OSP_FB_COLOR);
            buffer = floatFB_;
            break;
        }
    case BaseWindow::FRAMEBUFFER_DEPTH:
        {
            ospRenderFrame(fb_,renderer_,OSP_FB_DEPTH);
            depthFB_ = (float *)ospMapFrameBuffer(fb_, OSP_FB_DEPTH);
            buffer = depthFB_;
            format = GL_LUMINANCE;
            break;
        }
    default:
        glClearColor(1.f,0.f,0.f,1.f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
    if( applicationParameters_.isBenchmarking() ) fps_.doneRender();

    glDrawPixels(windowSize_.x, windowSize_.y, format, type, buffer);
    glutSwapBuffers();
    ospUnmapFrameBuffer(buffer, fb_);

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
    worldBounds_ = worldBounds;
    ospray::vec3f center = embree::center(worldBounds_);
    ospray::vec3f diag   = worldBounds_.size();
    diag         = max(diag,vec3f(0.3f*length(diag)));
    vec3f from   = center - .75f*vec3f(-.6*diag.x,-1.2*diag.y,.8*diag.z);
    from.z -= diag.z;
    ospray::vec3f dir = center - from;
    ospray::vec3f up  = viewPort_.up;

    viewPort_.at    = center;
    viewPort_.from  = from;
    viewPort_.up    = up;

    if (length(up) < DEFAULT_EPSILON)
        up = ospray::vec3f(0,0,1.f);

    viewPort_.frame.l.vy = normalize(dir);
    viewPort_.frame.l.vx = normalize(
                cross(viewPort_.frame.l.vy,up));
    viewPort_.frame.l.vz = normalize(
                cross(viewPort_.frame.l.vx,viewPort_.frame.l.vy));
    viewPort_.frame.p    = from;
    viewPort_.snapUp();
    viewPort_.modified = true;

    motionSpeed_ = length(diag) * 0.001f;
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
    case '1':
        ospSet3f(renderer_, "bgColor", 0.5f, 0.5f, 0.5f);
        break;
    case '2':
        ospSet3f(renderer_, "bgColor", 1.0f, 1.0f, 1.0f);
        break;
    case '3':
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
        break;
    }
    case 'o':
    {
        float aaStrength =
                renderingParameters_.getAmbientOcclusionStrength();
        aaStrength += 0.1f;
        if( aaStrength>1.f ) aaStrength=1.f;
        renderingParameters_.setAmbientOcclusionStrength( aaStrength );
        BRAYNS_INFO << "Ambient occlusion strength: " <<
                       aaStrength << std::endl;
        break;
    }
    case 'O':
    {
        float aaStrength =
                renderingParameters_.getAmbientOcclusionStrength();
        aaStrength -= 0.1f;
        if( aaStrength<0.f ) aaStrength=0.f;
        renderingParameters_.setAmbientOcclusionStrength( aaStrength );
        BRAYNS_INFO << "Ambient occlusion strength: " <<
                       aaStrength << std::endl;
        break;
    }
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
    case 'Z':
        if( frameBufferMode_==FRAMEBUFFER_DEPTH )
            frameBufferMode_ = FRAMEBUFFER_UCHAR;
        else
            frameBufferMode_ = FRAMEBUFFER_DEPTH;
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

void BaseWindow::generateMetaballs_( const float threshold )
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
