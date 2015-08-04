/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
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

#ifndef BraynsService_H
#define BraynsService_H

#include <brayns/common/types.h>
#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/parameters/RenderingParameters.h>
#include <brayns/common/parameters/GeometryParameters.h>

#ifdef BRAYNS_USE_RESTBRIDGE
#  include <restbridge/RestBridge.h>
#  include <zeq/zeq.h>
#  include <zeq/hbp/hbp.h>
typedef void* tjhandle;
#endif

#ifdef BRAYNS_USE_DEFLECT
#  include <brayns/common/extensions/DeflectManager.h>
#endif

namespace brayns
{

class DeflectManager;

/** Off-screen rendering service processing ZeroEQ and REST events
 * for interactive CPU based ray-tracing
 */
class BraynsService
{
public:

    BraynsService(const ApplicationParameters& applicationParameters);
    ~BraynsService();

    /** Sets the rendering parameters and renders the current frame
     */
    void render();

    /** Creates intial materials with random values
     */
    void createMaterials();

    /** Loads data specified in the command line arguments. This includes
     * SWC, H5, PDB and all mesh files supported by the assimp library
     */
    void loadData();

    /** Builds the OSPRay specific geometry from loaded data
     */
    void buildGeometry();

    /** Sets rendering parameters
     */
    void setRenderingParameters(
            const RenderingParameters& renderingParameters)
    {
        renderingParameters_ = renderingParameters;
    }

    /** Sets gemoetry parameters
     */
    void setGeometryParameters(
            const GeometryParameters& geometryParameters)
    {
        geometryParameters_ = geometryParameters;
    }

    /** Sets rendering quality. The quality parameter specifies the
     * number of images used for final compositing.
     */
    void setQuality( const int quality ) { quality_ = quality; }

    /** Processes events recieved via the ZeroEQ interface.
     */
    void processEvents();

#ifdef BRAYNS_USE_RESTBRIDGE
protected:
    /** Registers ZeroEQ application specific vocabulary
     */
    void registerVocabulary();

    /** Implements the CAMERA event
     */
    void onCamera( const zeq::Event& event );

    /** Implements the REQUEST event
     */
    void onRequest( const zeq::Event &event );

    /** Implements the EXIT event
     */
    void onExit( const zeq::Event &event );

    /** Encodes a raw image buffer into a JPEG compressed buffer
     */
    uint8_t* _encodeJpeg( const uint32_t width, const uint32_t height,
                          const uint8_t* rawData, unsigned long& jpSize );
#endif

protected:
    /** Saves current frame to disk. The filename is defined by a prefix and a
     * frame index (<prefix>_<frame>_%08d.ppm). The file uses the ppm encoding
     * and is written to the working folder of the application.
     *
     * @param frameIndex index of the current frame
     * @param prefix prefix used for the filename
     */
    void saveFrameToDisk(
            const size_t frameIndex,
            const std::string& prefix );

private:
    ApplicationParameters applicationParameters_;
    RenderingParameters renderingParameters_;
    GeometryParameters  geometryParameters_;

    uint32*        ucharFB_;
    OSPFrameBuffer fb_;
    int            frameNumber_;
    OSPRenderer    renderer_;
    OSPCamera      camera_;
    ospray::vec3f  cameraPos_;
    ospray::vec3f  cameraTarget_;
    OSPModel       model_;
    ospray::box3f  bounds_;
    ospray::vec2i  windowSize_;
    int            resampleSize_;
    int            quality_;
    bool           rendering_;
    bool           sceneModified_;

    // Geometry
    SpheresCollection   spheres_;
    CylindersCollection cylinders_;
    TrianglesCollection triangles_;
    MaterialsCollection materials_;

    // Lights
    ospray::vec3f    lightDirection_;
    OSPLight         light_;
    OSPData          lightData_;

#ifdef BRAYNS_USE_RESTBRIDGE
private:
    tjhandle handleCompress_;
    restbridge::RestBridge* restBridge_;
    zeq::Subscriber* rcSubscriber_;
    zeq::Publisher* rcPublisher_;
#endif

#ifdef BRAYNS_USE_DEFLECT
private:
    brayns::DeflectManager* deflectManager_;
#endif

};

}
#endif // BraynsService_H
