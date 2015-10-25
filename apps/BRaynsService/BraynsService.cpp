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
#include <brayns/common/extensions/ExtensionController.h>

#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#ifdef BRAYNS_USE_RESTBRIDGE
#  include <turbojpeg.h>
#  include <servus/uri.h>
#endif

#ifdef BRAYNS_USE_ASSIMP
#  include <brayns/common/loaders/MeshLoader.h>
#endif

namespace brayns
{

const float DEFAULT_EPSILON = 1e-2f;

BraynsService::BraynsService(
    const ApplicationParameters& applicationParameters )
 : applicationParameters_(applicationParameters),
    ucharFB_(NULL), fb_(NULL), frameNumber_(100000),
    renderer_(NULL), camera_(NULL), model_(NULL), quality_(1),
    rendering_(false), servingEvents_(false)
{
    windowSize_[0] = applicationParameters.getWindowWidth();
    windowSize_[1] = applicationParameters.getWindowHeight();

    renderer_ = ospNewRenderer(applicationParameters.getRenderer().c_str());
    Assert(renderer_ );

    model_ = ospNewModel();
    createMaterials();
    ospCommit(model_);

    camera_ = ospNewCamera("perspective");
    Assert2(camera_,"could not create camera");
    ospSetf(camera_, "aspect", float(windowSize_[0])/float(windowSize_[1]));
    ospCommit(camera_);

    Assert2(renderer_,"could not create renderer_");
    ospSetObject(renderer_,"world",model_);
    ospSetObject(renderer_,"camera",camera_);
    ospCommit(renderer_);

    fb_ = ospNewFrameBuffer(windowSize_,OSP_RGBA_I8,OSP_FB_COLOR|OSP_FB_ACCUM);
}

BraynsService::~BraynsService()
{
    if (fb_) ospFreeFrameBuffer(fb_);
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

    ospSet3f(renderer_, "bgColor", 0.f, 0.f, 0.f);
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
    ospSet1i(renderer_, "randomNumber", rand()%1000);
    ospSet1i(renderer_, "spp",
             renderingParameters_.getSamplesPerPixel());
    ospSet1i(renderer_, "electronShading",
             renderingParameters_.getElectronShading());
    ospSet1f(renderer_, "epsilon", DEFAULT_EPSILON);
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
    }
    ucharFB_ = (uint32 *)ospMapFrameBuffer(fb_);

    ospSet1i(renderer_, "moving", false);
    if( !extensionController_ )
    {
        ExtensionParameters extensionParameters = {
            renderer_, camera_, ucharFB_, fb_, windowSize_, bounds_, servingEvents_ };
        extensionController_.reset(new ExtensionController(
            applicationParameters_, extensionParameters ));
    }
    extensionController_->execute();
    servingEvents_ = extensionController_->getParameters().running;

    ospUnmapFrameBuffer(ucharFB_, fb_);
    rendering_ = false;
}

void BraynsService::saveFrameToDisk_(
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
            switch( i )
            {
            case 0:
                ospSet3f(materials_[i], "kd", 1.f, 1.f, 1.f );
                break;
            default:
                ospSet3f(materials_[i], "kd",
                         float(rand()%nbMaterials)/nbMaterials,
                         float(rand()%nbMaterials)/nbMaterials,
                         float(rand()%nbMaterials)/nbMaterials);
                break;
            }
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
    ospray::vec3f center = embree::center(bounds_);
    ospray::vec3f diag   = bounds_.size();
    diag         = max(diag,ospray::vec3f(0.5f*length(diag)));
    cameraPos_ = center;
    cameraPos_.x -= diag.x;
    cameraPos_.y += diag.y;
    cameraPos_.z -= diag.z;
    cameraTarget_ = center - cameraPos_;
    ospSetVec3f(camera_,"pos",cameraPos_);
    ospSetVec3f(camera_,"dir",cameraTarget_);
    ospCommit(camera_);
}

void BraynsService::processEvents()
{
    render();
}

}
