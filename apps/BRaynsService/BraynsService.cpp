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

#include "BraynsService.h"
#include <brayns/common/log.h>
#include <brayns/common/loaders/MorphologyLoader.h>
#include <brayns/common/loaders/ProteinLoader.h>

#include <unistd.h>
#include <dirent.h>

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

BraynsService::BraynsService(
        const ApplicationParameters& applicationParameters )
    : applicationParameters_(applicationParameters),
      ucharFB_(NULL), fb_(NULL), frameNumber_(100000),
      renderer_(NULL), camera_(NULL), model_(NULL), quality_(1),
      rendering_(false)
    #ifdef BRAYNS_USE_RESTBRIDGE
    , handleCompress_( tjInitCompress() )
    , restBridge_(NULL)
    , rcSubscriber_(NULL)
    , rcPublisher_(NULL)
    #endif // BRAYNS_USE_RESTBRIDGE
    #ifdef BRAYNS_USE_DEFLECT
    , deflectManager_(NULL)
    #endif
{
    windowSize_[0] = applicationParameters.getWindowWidth();
    windowSize_[1] = applicationParameters.getWindowHeight();

    renderer_ = ospNewRenderer(applicationParameters.getRenderer().c_str());
    Assert(renderer_ );

    // Background color
    ospSet3f(renderer_, "bgColor", 0.f, 0.f, 0.f);

    ospCommit(renderer_);

    model_ = ospNewModel();
    createMaterials();
    ospCommit(model_);

    camera_ = ospNewCamera("perspective");
    Assert2(camera_,"could not create camera");
    ospSet3f(camera_,"pos",0,0,-1000);
    ospSet3f(camera_,"dir",0,0, 1);
    ospSetf(camera_,"aspect",windowSize_.x/float(windowSize_.y));
    ospCommit(camera_);

    Assert2(renderer_,"could not create renderer_");
    ospSetObject(renderer_,"world",model_);
    ospSetObject(renderer_,"camera",camera_);
    ospCommit(renderer_);

    if (fb_) ospFreeFrameBuffer(fb_);
    fb_ = ospNewFrameBuffer(windowSize_,OSP_RGBA_I8,OSP_FB_COLOR|OSP_FB_ACCUM);
}

BraynsService::~BraynsService()
{
#ifdef BRAYNS_USE_RESTBRIDGE
    if( handleCompress_ )
        tjDestroy(handleCompress_);
    if( restBridge_)
        delete restBridge_;
    delete rcSubscriber_;
    delete rcPublisher_;
#endif
#ifdef BRAYNS_USE_DEFLECT
    delete deflectManager_;
#endif // BRAYNS_USE_DEFLECT
}

void BraynsService::loadData()
{
    bounds_ = embree::empty;
    Geometries geometries;

    if( applicationParameters_.getSWCFolder() != "" )
    {
        // Load SWC morphology files
        std::string folder = applicationParameters_.getSWCFolder();
        BRAYNS_INFO << "Loading SWC morphologies from " << folder << std::endl;
        struct dirent **namelist;
        int n = scandir(folder.c_str(), &namelist, 0, alphasort);
        int morphologyIndex(0);
        while(n--)
        {
            ospray::vec3f position(0.f,0.f,0.f);
            std::string filename(folder + '/' + namelist[n]->d_name);
            if( filename != "." && filename != ".." &&
                    filename.find(".swc") != std::string::npos)
            {
                BRAYNS_INFO << "- " << filename << std::endl;
                MorphologyLoader morphologyLoader(geometryParameters_);
                morphologyLoader.importSWCMorphologies(
                            filename, ++morphologyIndex,
                            position, geometries, bounds_ );
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    if( applicationParameters_.getPDBFolder() != "" )
    {
        // Load PDB File
        std::string folder = applicationParameters_.getPDBFolder();
        BRAYNS_INFO << "Loading PDB files from " << folder << std::endl;
        ProteinLoader proteinLoader(geometryParameters_);

        struct dirent **namelist;
        size_t n = scandir(folder.c_str(), &namelist, 0, alphasort);
        while(n--)
        {
            ospray::vec3f position(0.f,0.f,0.f);
            std::string filename(folder + '/' + namelist[n]->d_name);
            if( filename != "." && filename != ".."
                    && filename.find(".pdb") != std::string::npos )
            {
                BRAYNS_INFO << "- " << filename << std::endl;
                proteinLoader.importPDBFile(
                            filename, position, ms_atoms, 0,
                            materials_, geometries, bounds_);
            }
            free(namelist[n]);
        }
        free(namelist);

        for (size_t i=0;i<materials_.size();i++)
        {
            float r,g,b;
            proteinLoader.getMaterialKd(i,r,g,b);
            ospSet3f(materials_[i], "kd", r,g,b );
            ospCommit(materials_[i]);
        }
    }

#ifdef BRAYNS_USE_BBPSDK
    if( applicationParameters_.getH5Folder() != "" )
    {
        // Load h5 files
        std::string folder = applicationParameters_.getH5Folder();
        BRAYNS_INFO << "Loading H5 morphologies from " << folder << std::endl;
        ospray::vec3f position(0.f,0.f,0.f);
        MorphologyLoader morphologyLoader(geometryParameters_);
        morphologyLoader.importH5Morphologies(
                    folder, 0, position, geometries, bounds_);
    }
#endif

#ifdef BRAYNS_USE_ASSIMP
    if( applicationParameters_.getMeshFolder() != "" )
    {
        // Load mesh File
        std::string folder = applicationParameters_.getMeshFolder();
        BRAYNS_INFO << "Loading OBJ files from " << folder << std::endl;
        MeshLoader meshLoader;

        struct dirent **namelist;
        size_t n = scandir(folder.c_str(), &namelist, 0, alphasort);
        size_t meshIndex = 0;
        float scale = 0.1f;
        while(n--)
        {
            std::string filename = namelist[n]->d_name;
            if( filename != "." && filename != ".." &&
                    filename.find(".mtl") == std::string::npos )
            {
                filename = folder + '/' + filename;
                BRAYNS_INFO << "- [" << meshIndex << "] " <<
                               filename << std::endl;

                meshLoader.importMeshFromFile(
                            filename, scale,
                            triangles_, materials_, bounds_,
                            geometryParameters_.getColored() ? meshIndex:NO_MATERIAL);
                ++meshIndex;
            }
            free(namelist[n]);
        }
        free(namelist);
    }
#endif // USE_ASSIMP

    Geometries::const_iterator it(geometries.begin());
    while( it!=geometries.end() )
    {
        const Geometry& g(*it);
        switch(g.type)
        {
        case gt_sphere  :
            spheres_[g.materialId].push_back(
                        (brayns::Sphere){g.v0, g.r, g.frame});
            bounds_.extend(g.v0);
            break;
        case gt_cylinder:
            cylinders_[g.materialId].push_back(
                        (brayns::Cylinder){g.v0, g.v1, g.r, g.frame});
            bounds_.extend(g.v0);
            break;
        default:
            break;
        }
        ++it;
    }

    if( geometryParameters_.getTimedGeometry() )
        frameNumber_ = 0;
}

void BraynsService::render()
{
    rendering_ = true;
    ospSetVec3f(camera_,"pos",cameraPos_);
    ospSetVec3f(camera_,"dir",cameraTarget_ - cameraPos_);
    ospCommit(camera_);

    if (!fb_ || !renderer_)
    {
        BRAYNS_ERROR << "Oops..." << std::endl;
        return;
    }

    if( geometryParameters_.getTimedGeometry() )
    {
        frameNumber_ += geometryParameters_.getTimedGeometryIncrement();
        ospFrameBufferClear(fb_,OSP_FB_ACCUM);
    }

    if( renderingParameters_.getAmbientOcclusion() )
        ospSet3f(renderer_, "bgColor", 0.f, 0.f, 0.f);
    else
        ospSet3f(renderer_, "bgColor", 0.7f, 0.7f, 0.7f);

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
    ospSet1i(renderer_, "randomNumber", rand()%1000);
    ospSet1i(renderer_, "moving", false);
    ospSet1i(renderer_, "spp",
             renderingParameters_.getSamplesPerPixel());
    ospSet1i(renderer_, "electronShading",
             renderingParameters_.getElectronShading());
    ospCommit(renderer_);

    for( int i(0); i<quality_; ++i)
    {
        if( renderingParameters_.getDepthOfField() )
        {
            ospray::vec3f pos = cameraPos_;
            const float dofStrength = renderingParameters_.getDepthOfFieldStrength();
            pos.x += dofStrength*(rand()%100-50);
            pos.y += dofStrength*(rand()%100-50);
            pos.z += dofStrength*(rand()%100-50);
            ospSetVec3f(camera_,"pos",pos);
            ospCommit(camera_);
        }

        ospRenderFrame(fb_,renderer_,OSP_FB_COLOR|OSP_FB_ACCUM);
        ucharFB_ = (unsigned int *)ospMapFrameBuffer(fb_);
    }
#ifndef BRAYNS_USE_RESTBRIDGE
    saveFrameToDisk( 0, "Default" );
#endif
    ospUnmapFrameBuffer(ucharFB_, fb_);
#ifdef BRAYNS_USE_DEFLECT
    if( deflectManager_)
        deflectManager_->send(windowSize_, ucharFB_, true);
#endif
    rendering_ = false;
}

void BraynsService::saveFrameToDisk(
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

void BraynsService::createMaterials()
{
    const size_t nbMaterials = 200;
    for( size_t i=0; i<nbMaterials; ++i)
    {
        materials_[i] = ospNewMaterial(renderer_, "ExtendedOBJMaterial");
        if (materials_[i])
        {
            ospSet3f(materials_[i], "kd",
                     float(rand()%nbMaterials)/nbMaterials,
                     float(rand()%nbMaterials)/nbMaterials,
                     float(rand()%nbMaterials)/nbMaterials);
            ospCommit(materials_[i]);
        }
    }
}

void BraynsService::buildGeometry()
{
    if (!bounds_.empty())
    {
        OSPGeometry* extendedSpheres = new OSPGeometry[materials_.size()];
        OSPGeometry* extendedCylinders = new OSPGeometry[materials_.size()];
        for( size_t i=0; i<materials_.size(); ++i )
        {
            extendedSpheres[i] = ospNewGeometry("extendedspheres");
            Assert(extendedSpheres[i]);

            // Extended spheres
            OSPData data = ospNewData(
                        spheres_[i].size()*5, OSP_FLOAT, &spheres_[i][0]);
            ospSetObject(extendedSpheres[i],
                         "extendedspheres", data);
            ospSet1i(extendedSpheres[i],
                     "bytes_per_extended_sphere", 5*sizeof(float));
            ospSet1i(extendedSpheres[i],
                     "offset_radius", 3*sizeof(float));
            ospSet1i(extendedSpheres[i],
                     "offset_frame", 4*sizeof(float));

            if (materials_[i])
                ospSetMaterial(extendedSpheres[i], materials_[i]);

            ospCommit(extendedSpheres[i]);
            ospAddGeometry(model_, extendedSpheres[i]);

            // Extended cylinders
            extendedCylinders[i] = ospNewGeometry("extendedcylinders");
            Assert(extendedCylinders[i]);

            data = ospNewData(
                        cylinders_[i].size()*8, OSP_FLOAT, &cylinders_[i][0]);
            ospSetObject(extendedCylinders[i],
                         "extendedcylinders", data);
            ospSet1i(extendedCylinders[i],
                     "bytes_per_extended_cylinder", 8*sizeof(float));
            ospSet1i(extendedCylinders[i],
                     "offset_frame", 7*sizeof(float));

            if (materials_[i])
                ospSetMaterial(extendedCylinders[i], materials_[i]);

            ospCommit(extendedCylinders[i]);
            ospAddGeometry(model_, extendedCylinders[i]);

            // Triangles
            OSPGeometry triangles = ospNewTriangleMesh();
            Assert(triangles);
            OSPData vertices = ospNewData(
                        triangles_[i].vertex.size(),OSP_FLOAT3A,
                        &triangles_[i].vertex[0],OSP_DATA_SHARED_BUFFER);
            OSPData normals  = ospNewData(
                        triangles_[i].normal.size(),OSP_FLOAT3A,
                        &triangles_[i].normal[0],OSP_DATA_SHARED_BUFFER);
            OSPData indices  = ospNewData(
                        triangles_[i].index.size(),OSP_INT3,
                        &triangles_[i].index[0],OSP_DATA_SHARED_BUFFER);
            OSPData colors   = ospNewData(
                        triangles_[i].color.size(),OSP_FLOAT3A,
                        &triangles_[i].color[0],OSP_DATA_SHARED_BUFFER);
            OSPData texcoord = ospNewData(
                        triangles_[i].texcoord.size(),OSP_FLOAT2,
                        &triangles_[i].texcoord[0],OSP_DATA_SHARED_BUFFER);
            ospSetObject(triangles,"vertex",vertices);
            ospSetObject(triangles,"index",indices);
            ospSetObject(triangles,"vertex.normal",normals);
            ospSetObject(triangles,"vertex.color",colors);
            ospSetObject(triangles,"vertex.texcoord",texcoord);
            ospSet1i(triangles, "alpha_type", 0);
            ospSet1i(triangles, "alpha_component", 4);

            if (materials_[i])
                ospSetMaterial(triangles, materials_[i]);

            ospCommit(triangles);
            ospAddGeometry(model_, triangles);
        }
        delete extendedSpheres;
        delete extendedCylinders;
    }
    ospCommit(model_);

    std::string rendererType = applicationParameters_.getRenderer();
    if (rendererType == "exobj" || rendererType == "obj")
    {
        lightDirection_ = ospray::vec3f(1.f, -1.f, 1.f);
        light_ = ospNewLight(renderer_, "DirectionalLight");
        ospSet3f(light_, "color", 0.8f, 0.8f, 0.8f);
        ospSet3f(light_, "direction",
                 lightDirection_.x, lightDirection_.y, lightDirection_.z);
        ospSet3f(light_, "position", 0.f, 0.f, 0.f);
        ospSet1f(light_, "intensity", 1.f);
        ospSet1f(light_, "halfAngle", 15.f);
        ospSet1f(light_, "range", 50.f);
        ospCommit(light_);

        lightData_ = ospNewData(1,OSP_OBJECT,&light_ );
        ospCommit(lightData_);
        ospSetData(renderer_,"lights",lightData_);
    }
    ospCommit(renderer_);

    // Initial camera position
    cameraTarget_ = embree::center(bounds_);
    ospray::vec3f diag   = bounds_.size();
    diag         = max(diag,ospray::vec3f(0.3f*length(diag)));
    cameraPos_.z -= diag.z*0.5f;
    ospSetVec3f(camera_,"pos",cameraPos_);
    ospSetVec3f(camera_,"dir",cameraTarget_ - cameraPos_);
    ospCommit(camera_);

}

void BraynsService::processEvents()
{
#ifdef BRAYNS_USE_DEFLECT
    if( !deflectManager_ && applicationParameters_.getDeflectHostname() != "" )
    {
        deflectManager_ = new DeflectManager(
                    applicationParameters_.getDeflectHostname(),
                    applicationParameters_.getDeflectStreamname(),
                    true, 100 );
    }
    if( deflectManager_)
    {
        ospray::vec3f newPos = cameraPos_;
        deflectManager_->handleTouchEvents(newPos );
        if( newPos != cameraPos_ )
        {
            cameraPos_ = newPos;
            ospSetVec3f(camera_, "pos", cameraPos_);
            ospCommit(camera_);
        }
    }
#endif
#ifdef BRAYNS_USE_RESTBRIDGE
    if(!rcSubscriber_)
    {
        BRAYNS_INFO << "Registering handlers " << std::endl;
        rcSubscriber_ = new zeq::Subscriber(
                    servus::URI(applicationParameters_.getZeqSchema() +
                                "cmd://" ));

        rcSubscriber_->registerHandler(
                    zeq::hbp::EVENT_CAMERA,
                    boost::bind( &BraynsService::onCamera,
                                 this, _1 ));
        rcSubscriber_->registerHandler(
                    zeq::vocabulary::EVENT_REQUEST,
                    boost::bind( &BraynsService::onRequest,
                                 this, _1 ));
        rcSubscriber_->registerHandler(
                    zeq::vocabulary::EVENT_EXIT,
                    boost::bind( &BraynsService::onExit,
                                 this, _1 ));
    }
    if(!rcPublisher_)
    {
        rcPublisher_ = new zeq::Publisher(
                    servus::URI( applicationParameters_.getZeqSchema() + "resp://" ));
    }

    if( rcSubscriber_ )
    {
        while( rcSubscriber_->receive(10) );

        rcPublisher_->publish( zeq::Event( zeq::vocabulary::EVENT_HEARTBEAT ));
    }

    /* The RESTBridge should only be created once the zeq layer is fully
     * initialized
     */
    if( !restBridge_ )
    {
        restBridge_ = new restbridge::RestBridge(
                    applicationParameters_.getRESTHostname(),
                    applicationParameters_.getRESTPort());

        restBridge_->run( applicationParameters_.getZeqSchema() );
        BRAYNS_INFO << "Initializing restBridge " <<
                       applicationParameters_.getZeqSchema() <<
                       "://" << applicationParameters_.getRESTHostname() <<
                       ":" << applicationParameters_.getRESTPort() <<
                       std::endl;
    }
#endif
    render();
}

#ifdef BRAYNS_USE_RESTBRIDGE
void BraynsService::registerVocabulary()
{
    BRAYNS_INFO << "Registering application vocabulary" << std::endl;
    if( rcPublisher_ )
    {
        zeq::EventDescriptors vocabulary;
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
        vocabulary.push_back( zeq::EventDescriptor(
                                  zeq::hbp::IMAGEJPEG,
                                  zeq::hbp::EVENT_IMAGEJPEG,
                                  zeq::hbp::SCHEMA_IMAGEJPEG,
                                  zeq::PUBLISHER ) );
        const zeq::Event& vocEvent =
                zeq::vocabulary::serializeVocabulary( vocabulary );
        rcPublisher_->publish( vocEvent );
        BRAYNS_INFO << "Vocabulary registered" << std::endl;
    }
}

void BraynsService::onCamera( const zeq::Event& event )
{
    if( event.getType() != zeq::hbp::EVENT_CAMERA )
        return;

    const std::vector< float >& matrix =
            zeq::hbp::deserializeCamera(event);
    cameraPos_ = ospray::vec3f(matrix[0], matrix[1], matrix[2]);
    BRAYNS_DEBUG << "Camera position: " <<
                    matrix[0] << "," << matrix[1] << "," <<
                    matrix[2] << std::endl;
    cameraPos_ = cameraPos_ * bounds_.size();

    ospSetVec3f(camera_, "pos", cameraPos_);
    ospSetVec3f(camera_, "dir", cameraTarget_ - cameraPos_);
    ospCommit(camera_);
    ospFrameBufferClear(fb_,OSP_FB_ACCUM);
}

void BraynsService::onRequest( const zeq::Event& event )
{
    const zeq::uint128_t& eventType =
            zeq::vocabulary::deserializeRequest( event );

    if( eventType == zeq::hbp::EVENT_IMAGEJPEG )
    {
        if( !rendering_ ) render();
        unsigned long jpegSize =
                windowSize_.x * windowSize_.y * sizeof(uint32);

        uint8_t* jpegData = _encodeJpeg(
                    (uint32_t)windowSize_.x, (uint32_t)windowSize_.y,
                    (uint8_t*)ucharFB_, jpegSize);

        const zeq::hbp::data::ImageJPEG image( jpegSize, jpegData );
        const zeq::Event& image_event =
                zeq::hbp::serializeImageJPEG( image );
        rcPublisher_->publish( image_event );
    }
    else if( eventType == zeq::vocabulary::EVENT_VOCABULARY )
    {
        BRAYNS_INFO << "Application vocabulary now registered" <<
                       std::endl;
        registerVocabulary();
    }
    else if( eventType == zeq::hbp::EVENT_CAMERA )
    {
        std::vector<float> matrix;
        vec3f pos = cameraPos_ / bounds_.size();
        vec3f target = cameraTarget_ / bounds_.size();
        matrix.push_back(-pos.x);
        matrix.push_back(pos.y);
        matrix.push_back(-pos.z);
        matrix.push_back(-target.x);
        matrix.push_back(target.y);
        matrix.push_back(-target.z);
        for( int i(0); i<10; ++i) matrix.push_back(0);
        const zeq::Event& camera_event =
                zeq::hbp::serializeCamera( matrix );
        rcPublisher_->publish( camera_event );
    }
}

void BraynsService::onExit( const zeq::Event &event )
{
    if( event.getType() != zeq::vocabulary::EVENT_EXIT ) return;
    exit(1);
}


uint8_t* BraynsService::_encodeJpeg( const uint32_t width,
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
        BRAYNS_ERROR << "libjpeg-turbo image conversion failure" <<
                        std::endl;
        return 0;
    }
    return static_cast<uint8_t *>(tjJpegBuf);
}
#endif

}
