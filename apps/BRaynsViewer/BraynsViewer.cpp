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

#include "BraynsViewer.h"
#include <brayns/common/loaders/MorphologyLoader.h>
#include <brayns/common/loaders/ProteinLoader.h>
#include <brayns/common/log.h>

#ifdef BRAYNS_USE_ASSIMP
#  include <brayns/common/loaders/MeshLoader.h>
#endif

#include <dirent.h>

namespace brayns
{

BraynsViewer::BraynsViewer( const ApplicationParameters& applicationParameters )
    : BaseWindow( applicationParameters,
                  FRAMEBUFFER_NONE, INSPECT_CENTER_MODE,
                  INSPECT_CENTER_MODE|MOVE_MODE)
{
    const std::string cameraName = applicationParameters.getCamera();

    camera_ = ospNewCamera(cameraName.c_str());
    Assert2(camera_,"could not create camera");
    ospSet3f(camera_,"pos",0,0,-1);
    ospSet3f(camera_,"dir",0,0, 1);

    rendererType_ = applicationParameters.getRenderer();
    renderer_ = ospNewRenderer(rendererType_.c_str());
    Assert(renderer_ );

    model_ = ospNewModel();
    createMaterials();
    ospCommit(model_);

    Assert2(renderer_,"could not create renderer_");
    ospSetObject(renderer_,"world",model_);
    ospSetObject(renderer_,"camera",camera_);
    ospSet3f(renderer_, "bgColor", 0.5f, 0.5f, 0.5f);
    ospCommit(renderer_);
}

void BraynsViewer::reshape(const ospray::vec2i &newSize)
{
    BaseWindow::reshape( newSize );

    if (fb_)
        ospFreeFrameBuffer( fb_ );

    fb_ = ospNewFrameBuffer(newSize, OSP_RGBA_I8,
                            OSP_FB_COLOR|OSP_FB_ACCUM|OSP_FB_ALPHA);

    ospSetf( camera_, "aspect", viewPort_.aspect );
    ospCommit(camera_);
}

void BraynsViewer::keypress(char key, const ospray::vec2f where)
{
    switch (key)
    {
    case 'R':
        frameNumber_ = 100000;
        break;
    case 'r':
        frameNumber_ = 0;
        break;
    case 'e':
        ospSet1f(materials_[0], "r", 0.9f+float(rand()%10)/100.f);
        ospSet1f(materials_[0], "d", float(rand()%20)/100.f);
        ospCommit(materials_[0]);
        break;
    case 'g':
        geometryParameters_.setTimedGeometry(
                    !geometryParameters_.getTimedGeometry());
        break;
    case ']':
        frameNumber_ += geometryParameters_.getTimedGeometryIncrement();
        break;
    case '[':
        frameNumber_ -= geometryParameters_.getTimedGeometryIncrement();
        if( frameNumber_<0 ) frameNumber_ = 0;
        break;
    case '*':
        renderingParameters_.display();
        geometryParameters_.display();
        break;
    case 'K':
        ospSet1f(light_, "intensity", rand()%255/255.f);
        ospCommit(light_);
        break;
    default:
        BaseWindow::keypress(key,where);
    }
    ospFrameBufferClear(fb_,OSP_FB_ACCUM);
}

void BraynsViewer::display()
{
    if (!fb_ || !renderer_)
        return;

    if( geometryParameters_.getTimedGeometry())
    {
        frameNumber_ += geometryParameters_.getTimedGeometryIncrement();
        ospFrameBufferClear(fb_,OSP_FB_ACCUM);
    }

    ucharFB_ = (unsigned int *)ospMapFrameBuffer(fb_);
    frameBufferMode_ = FRAMEBUFFER_UCHAR;
    BaseWindow::display();
    ospUnmapFrameBuffer(ucharFB_, fb_);

    char title[1000];
    sprintf( title, "BRayns Viewer - Interactive Ray-Tracing @ %f fps",
             fps_.getFPS());
    setTitle(title);
    forceRedraw();
}

void BraynsViewer::createMaterials()
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

void BraynsViewer::loadData()
{
    resetBounds();
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
                filename.find(".swc") != std::string::npos )
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
            if( filename != "." && filename != ".." &&
                filename.find(".pdb") != std::string::npos )
            {
                BRAYNS_INFO << "- " << filename << std::endl;
                proteinLoader.importPDBFile(
                            filename, position, ms_atoms, 0,
                            materials_, geometries, bounds_);
            }
            free(namelist[n]);
        }
        free(namelist);

        // Overwrite existing materials with the ones defined by the
        // protein loader
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

    // Build OSPray Geometries
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

void BraynsViewer::buildGeometry()
{
    if( metaballsGridDimension_!=0 )
        generateMetaballs(0.01);

    if (!bounds_.empty())
    {
        OSPGeometry* extendedSpheres = new OSPGeometry[materials_.size()];
        OSPGeometry* extendedCylinders = new OSPGeometry[materials_.size()];
        OSPGeometry* extendedCones = new OSPGeometry[materials_.size()];
        for( size_t i=0; i<materials_.size(); ++i )
        {
            // Extended spheres
            extendedSpheres[i] = ospNewGeometry("extendedspheres");
            Assert(extendedSpheres[i]);

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

            // Extended cones
            extendedCones[i] = ospNewGeometry("extendedcones");
            Assert(extendedCones[i]);

            data = ospNewData(
                        cones_[i].size()*9, OSP_FLOAT, &cones_[i][0]);
            ospSetObject(extendedCones[i],
                         "extendedcones", data);
            ospSet1i(extendedCones[i],
                     "bytes_per_extended_cone", 9*sizeof(float));
            ospSet1i(extendedCones[i],
                     "offset_frame", 8*sizeof(float));

            if (materials_[i])
                ospSetMaterial(extendedCones[i], materials_[i]);

            ospCommit(extendedCones[i]);
            ospAddGeometry(model_, extendedCones[i]);

            // Streamlines
            OSPGeometry streamlines = ospNewGeometry("streamlines");
            Assert(streamlines);
            OSPData vertex = ospNewData(
                        streamlines_[i].vertex.size(),OSP_FLOAT3A,
                        &streamlines_[i].vertex[0]);
            OSPData index  = ospNewData(
                        streamlines_[i].index.size(),OSP_UINT,
                        &streamlines_[i].index[0]);
            ospSetObject(streamlines,
                         "vertex",vertex);
            ospSetObject(streamlines,
                         "index",index);
            ospSet1f(streamlines,
                     "radius",streamlines_[i].radius);
            if(materials_[i])
                ospSetMaterial(streamlines,materials_[i]);
            ospCommit(streamlines);
            ospAddGeometry(model_,streamlines);

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
        delete extendedCones;
    }

    ospCommit(model_);

    if (rendererType_ == "exobj" || rendererType_ == "obj")
    {
        lightDirection_ = ospray::vec3f(1.f, -1.f, 1.f);
        light_ = ospNewLight(renderer_, "DirectionalLight");
        ospSet3f(light_, "color", 1.f, 1.f, 1.f);
        ospSet3f(light_, "direction",
                 lightDirection_.x, lightDirection_.y, lightDirection_.z);
        ospSet1f(light_, "intensity", 1.f);
        ospSet3f(light_, "position", 0.f, 0.f, 0.f);
        ospSet1f(light_, "halfAngle", 15.f);
        ospSet1f(light_, "range", 500.f);
        ospCommit(light_);

        lightData_ = ospNewData(1,OSP_OBJECT,&light_ );
        ospCommit(lightData_);
        ospSetData(renderer_,"lights",lightData_);
    }
    ospCommit(renderer_);

    setWorldBounds(bounds_);
}

void BraynsViewer::buildEnvironment( const ospray::vec3f& scale )
{
    switch( applicationParameters_.getSceneEnvironment() )
    {
    case seNone:
        break;
    case seGround:
    {
        // Ground
        ospray::vec3f s = bounds_.size();
        ospray::vec3f c = (bounds_.lower + bounds_.upper) / 2.f;
        ospray::vec3i i;
        size_t material = materials_.size()-1;
        size_t meshIndex = triangles_[material].index.size();
        ospSet3f(materials_[material], "kd", 0, 0 ,0);
        ospCommit(materials_[material]);
        ospray::vec3fa v;
        ospray::vec3fa n = {0,1,0};
        v = ospray::vec3fa( c.x-s.x*scale.x, c.y-s.y*scale.y, c.z-s.z*scale.z);
        triangles_[material].vertex.push_back(v);
        triangles_[material].normal.push_back(n);
        v = ospray::vec3fa( c.x+s.x*scale.x, c.y-s.y*scale.y, c.z-s.z*scale.z);
        triangles_[material].vertex.push_back(v);
        triangles_[material].normal.push_back(n);
        v = ospray::vec3fa( c.x+s.x*scale.x, c.y-s.y*scale.y, c.z+s.z*scale.z);
        triangles_[material].vertex.push_back(v);
        triangles_[material].normal.push_back(n);
        i = ospray::vec3i(meshIndex, meshIndex+1, meshIndex+2);
        triangles_[material].index.push_back(i);
        meshIndex += 3;
        v = ospray::vec3fa( c.x+s.x*scale.x, c.y-s.y*scale.y, c.z+s.z*scale.z);
        triangles_[material].vertex.push_back(v);
        triangles_[material].normal.push_back(n);
        v = ospray::vec3fa( c.x-s.x*scale.x, c.y-s.y*scale.y, c.z+s.z*scale.z);
        triangles_[material].vertex.push_back(v);
        triangles_[material].normal.push_back(n);
        v = ospray::vec3fa( c.x-s.x*scale.x, c.y-s.y*scale.y, c.z-s.z*scale.z);
        triangles_[material].vertex.push_back(v);
        triangles_[material].normal.push_back(n);
        i = ospray::vec3i(meshIndex, meshIndex+1, meshIndex+2);
        triangles_[material].index.push_back(i);
        break;
    }
    case seBox:
    {
        BRAYNS_ERROR << "Box environement is not implemented" << std::endl;
        break;
    }
    }
}

}
