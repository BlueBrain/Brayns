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

#include /*embree*/"common/math/affinespace.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

#include <brayns/common/types.h>
#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/parameters/RenderingParameters.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <brayns/common/ui/manipulators/InspectCenterManipulator.h>
#include <brayns/common/ui/manipulators/FlyingModeManipulator.h>

namespace brayns
{

//! dedicated namespace for 3D glut viewer widget
/*! initialize everything GLUT-related */
void initGLUT(int32 *ac, const char **av);

/*! switch over to GLUT for control flow. This functoin will not return */
void runGLUT();

using embree::AffineSpace3fa;

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
    void startRender() { frameStartTime = ospray::getSysTime(); }
    void doneRender() {
        double seconds = ospray::getSysTime() - frameStartTime;
        smooth_nom = smooth_nom * 0.8f + seconds;
        smooth_den = smooth_den * 0.8f + 1.f;
    }
    double getFPS() const { return smooth_den / smooth_nom; }
};

class BaseWindow
{
public:

    static BaseWindow *activeWindow_;

    typedef enum
    {
        FRAMEBUFFER_UCHAR,
        FRAMEBUFFER_FLOAT,
        FRAMEBUFFER_DEPTH,
        FRAMEBUFFER_NONE
    } FrameBufferMode;

    typedef enum
    {
        MOVE_MODE           =(1<<0),
        INSPECT_CENTER_MODE =(1<<1)
    } ManipulatorMode;

    BaseWindow(const ApplicationParameters& applicationParameters,
               const FrameBufferMode frameBufferMode,
               const ManipulatorMode initialManipulator=INSPECT_CENTER_MODE,
               int allowedManipulators=INSPECT_CENTER_MODE|MOVE_MODE);
    virtual ~BaseWindow();

    /*! internal viewPort class */
    class ViewPort
    {
    public:
        bool modified; /* the viewPort will set this flag any time any of
                          its values get changed. */

        ospray::vec3f from;
        ospray::vec3f up;
        ospray::vec3f at;
        /*! opening angle, in radians, along Y direction */
        float openingAngle;
        /*! aspect ration i Y:X */
        float aspect;
        // float focalDistance;

        /*! camera frame in which the Y axis is the depth axis, and X
          and Z axes are parallel to the screen X and Y axis. The frame
          itself remains normalized. */
        AffineSpace3fa frame;

        /*! set 'up' vector. if this vector is '0,0,0' the viewer will
         *not* apply the up-vector after camera manipulation */
        void snapUp();

        ViewPort();
    };

    // static InspectCenter INSPECT_CENTER;
    InspectCenterManipulator *inspectCenterManipulator;
    FlyingModeManipulator *flyingModeManipulator;

    /*! current manipulator */
    AbstractManipulator *manipulator;

    /*! size we'll create a window at */
    static ospray::vec2i defaultInitSize;

    /*! set a default camera position that views given bounds from the
        top left front */
    virtual void setWorldBounds(const ospray::box3f &worldBounds);

    /*! tell GLUT that this window is 'dirty' and needs redrawing */
    virtual void forceRedraw();

    /*! set window title */
    void setTitle(const char *title);

    /*! set window title */
    void setTitle(const std::string &title) { setTitle(title.c_str()); }

    /*! set viewport to given values */
    ViewPort& getViewPort() { return viewPort_; }
    void setViewPort(
            const ospray::vec3f from,
            const ospray::vec3f at,
            const ospray::vec3f up);

    // ------------------------------------------------------------------
    // event handling - override this to change this widgets behavior
    // to input events
    // ------------------------------------------------------------------
    virtual void mouseButton(
            int32 which,
            bool released,
            const ospray::vec2i& pos);

    virtual void motion(
            const ospray::vec2i& pos);

    virtual void reshape(
            const ospray::vec2i& newSize);

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
    void drawPixels(const ospray::vec3fa *framebuffer);

    // ------------------------------------------------------------------
    // camera helper code
    // ------------------------------------------------------------------
    void snapUp();
    /*! set 'up' vector. if this vector is '0,0,0' the viewer will
       *not* apply the up-vector after camera manipulation */
    virtual void setZUp(const ospray::vec3f &up);
    void noZUp() { setZUp(ospray::vec3f(0.f)); }

    friend void glut3dReshape(int32 x, int32 y);
    friend void glut3dDisplay(void);
    friend void glut3dKeyboard(char key, int32 x, int32 y);
    friend void glut3dIdle(void);
    friend void glut3dMotionFunc(int32 x, int32 y);
    friend void glut3dMouseFunc(int32 whichButton, int32 released,
                                int32 x, int32 y);

    virtual void keypress(char key, const ospray::vec2f where);
    virtual void specialkey(int32 key, const ospray::vec2f where);

    void setRenderingParameters(
            const RenderingParameters& renderingParameters )
    {
        renderingParameters_ = renderingParameters;
    }

    void setGeometryParameters(
            const GeometryParameters& geometryParameters )
    {
        geometryParameters_ = geometryParameters;
    }

    void setMetaballsGridDimension( size_t metaballsGridDimension )
    {
        metaballsGridDimension_ = metaballsGridDimension;
    }

    void resetBounds();
    ospray::box3f getBounds() const;

    void resetFrame() { frameNumber_ = 0; }

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

    /*! recompute current viewPort's frame from cameras 'from',
     * 'at', 'up' values. */
    void computeFrame();

    /*! last mouse screen position of mouse before current motion */
    ospray::vec2i getLastMousePos()        { return lastMousePos_; }

    /*! current screen position of mouse */
    ospray::vec2i getCurrentMousePos()     { return currMousePos_; }
    ospray::int64 getLastButtonState()     { return lastButtonState_; }
    ospray::int64 getCurrentButtonState()  { return currButtonState_; }
    ospray::int64 getCurrentModifiers()    { return currModifiers_; }
    ospray::vec3f getUpVectorFromCmdLine() { return upVectorFromCmdLine_; }

    /*!< world bounds, to automatically set viewPort lookat, mouse speed, etc */
    ospray::box3f getWorldBounds()         { return worldBounds_; }

    float getRotateSpeed() { return rotateSpeed_; }
    float getMotionSpeed() { return motionSpeed_; }

    uint32* getFrameBuffer() { return ucharFB_; }

    ospray::vec2i getWindowSize() { return windowSize_; }

protected:

    void generateMetaballs_( float threshold );

    ospray::vec2i lastMousePos_; /*! last mouse screen position of mouse before
                                    current motion */
    ospray::vec2i currMousePos_; /*! current screen position of mouse */
    ospray::int64 lastButtonState_;
    ospray::int64 currButtonState_;
    ospray::int64 currModifiers_;
    ospray::vec3f upVectorFromCmdLine_;
    ospray::box3f worldBounds_; /*!< world bounds, to automatically set viewPort
                                    lookat, mouse speed, etc */

    /*! camera speed modifier - affects how many units the camera _moves_ with
     * each unit on the screen */
    float motionSpeed_;
    /*! camera rotation speed modifier - affects how many units the camera
     * _rotates_ with each unit on the screen */
    float rotateSpeed_;

    /*! uchar[4] RGBA-framebuffer, if applicable */
    uint32* ucharFB_;
    /*! float[4] RGBA-framebuffer, if applicable */
    ospray::vec3fa* floatFB_;
    /*! floating point depth framebuffer, if applicable */
    float* depthFB_;

    FrameBufferMode frameBufferMode_;

    ApplicationParameters applicationParameters_;
    RenderingParameters renderingParameters_;
    GeometryParameters  geometryParameters_;

    ViewPort       viewPort_;
    int32          windowID_;
    ospray::vec2i  windowSize_;

    bool           fullScreen_;
    OSPFrameBuffer fb_;
    OSPRenderer    renderer_;
    OSPCamera      camera_;
    OSPModel       model_;
    ospray::box3f  bounds_;

    int            resampleSize_;
    int            frameNumber_;
    int            frameCounter_;
    size_t         metaballsGridDimension_;

    FPSCounter fps_;

    ospray::vec3f    lightDirection_;
    OSPLight         light_;
    OSPData          lightData_;

    // Model
    SpheresCollection   spheres_;
    CylindersCollection cylinders_;
    TrianglesCollection triangles_;
    ConesCollection cones_;
    StreamLinesCollection streamlines_;
    MaterialsCollection materials_;

    // OSP geometry
    OSPGeometryCollections extendedSpheres_;
    OSPGeometryCollections extendedCylinders_;

    OSPDataCollections spheresData_;
    OSPDataCollections cylindersData_;

    OSPGeometryCollections ospTriangles_;
    OSPDataCollections ospVertices_;
    OSPDataCollections ospIndices_;
    OSPDataCollections ospNormals_;
    OSPDataCollections ospColors_;

    bool running_;

private:

    void setRendererParameters_();

    std::unique_ptr< ExtensionController > extensionController_;
};

std::ostream &operator<<(std::ostream &o, const BaseWindow::ViewPort &cam);

}

#endif
