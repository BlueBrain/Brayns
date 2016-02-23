/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "MorphologyLoader.h"

#include <brayns/common/log.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/Cylinder.h>

#include <sstream>

#ifdef BRAYNS_USE_BBPSDK
#  include <BBP/Readers/Morphology_Reader.h>
#  include <BBP/Containers/Morphologies.h>
#  include <BBP/Soma.h>
#  include <boost/foreach.hpp>
#endif

namespace brayns
{

MorphologyLoader::MorphologyLoader(
        const GeometryParameters& geometryParameters )
    : _geometryParameters(geometryParameters)
{
}

bool MorphologyLoader::importMorphologies(
        MorphologyFileFormat fileFormat,
        const std::string& filename,
        int morphologyIndex,
        const Vector3f& position,
        PrimitivesMap& primitives,
        Boxf& bounds)
{
    switch(fileFormat)
    {
    case MFF_SWC:
        return _importSWCFile(filename, morphologyIndex, position, primitives, bounds);
        break;
    case MFF_H5:
        return _importH5File(filename, morphologyIndex, position, primitives, bounds);
        break;
    default:
        BRAYNS_ERROR << "Unsupported file format. " <<
            filename << " could not be imported" <<
            std::endl;
    }
    return false;
}

#ifdef BRAYNS_USE_BBPSDK
bool MorphologyLoader::_importH5File(
        const std::string &filename,
        int morphologyIndex,
        const Vector3f& position,
        PrimitivesMap& primitives,
        Boxf& bounds
        )
{
    Morphologies morphologies;

    Vector3f somaPosition(0.f,0.f,0.f);
    Vector3f randomPosition(0.f,0.f,0.f);
    bbp::Morphology_Reader reader;
    reader.open(filename.c_str());
    if( !reader.is_open() )
        return false;
    else
    {
        bbp::Morphologies bbpMorphologies;
        reader.read( bbpMorphologies );
        reader.close();

        bbp::Morphologies::const_iterator it =  bbpMorphologies.begin();
        while( it != bbpMorphologies.end() )
        {
            size_t frameId = 0;
            size_t count_sections = 0;
            const size_t material =
                ( _geometryParameters.getColorScheme() == CS_NEURON_BY_ID) ?
                ( morphologyIndex%100 ) : 0;

            const bbp::Morphology morphology = (*it);
            const bbp::Sections& sections = morphology.sections();
            for(bbp::Section section: sections )
            {
                // Soma
                const bbp::Soma soma = morphology.soma();
                const bbp::Vector3f pos = soma.position();
                Vector3f v(pos[0],pos[1],pos[2]);
                v += randomPosition+position;
                if( count_sections == 0 )
                {
                    somaPosition = v;

                    primitives[material].push_back(SpherePtr(
                        new Sphere(material, v,
                            soma.max_radius()*_geometryParameters.getRadius(),
                            static_cast<float>(frameId))));
                    bounds.merge(v);
                }
                Vector3f va = v;

                size_t count_segments = 0;
                const bbp::Segments& segments = section.segments();
                for(bbp::Segment segment: segments )
                {
                    const bbp::Vector3f b = segment.end().center();
                    bbp::Cross_Section cs = segment.cross_section(1.0);
                    Vector3f vb(b[0],b[1],b[2]);
                    vb += randomPosition+position;

                    const float r = cs.radius()*_geometryParameters.getRadius();

                    frameId = Vector3f(vb-v).length();

                    // Branches
                    primitives[material].push_back(SpherePtr(
                        new Sphere(material, vb, r, static_cast<float>(frameId))));

                    primitives[material].push_back(CylinderPtr(
                        new Cylinder(material, va, vb, r, static_cast<float>(frameId))));

                    bounds.merge(va);
                    bounds.merge(vb);

                    va = vb;
                    ++count_segments;
                }
                ++count_sections;
            }
            ++morphologyIndex;
            ++it;
        }
    }

    Branches branches;
    Morphologies::const_iterator itm=morphologies.begin();
    while( itm!=morphologies.end())
    {
        const Morphology& a = (*itm).second;
        if( a.children.size()>1 )
        {
            for( size_t i=0; i<a.children.size(); ++i )
            {
                Branch b;
                b.segments.push_back(a.children[i]);
                branches.push_back(b);
            }
        }
        ++itm;
    }

    Branches::const_iterator itb=branches.begin();
    while( itb!=branches.end())
    {
        int segment(0);
        Vector3f vb;
        const Branch& branch = (*itb);
        Morphology& morphology = morphologies[branch.segments[0]];
        bool carryon = true;
        while( carryon )
        {
            Vector3f va(morphology.x, morphology.y, morphology.z);

            if( segment>0 )
            {
                primitives[morphology.branch].push_back(SpherePtr(
                    new Sphere(morphology.branch, va,
                        morphology.radius*_geometryParameters.getRadius(),
                        static_cast<float>(morphology.frame))));
                bounds.merge(va);
            }
            vb = va;
            ++segment;

            if(morphology.children.size()==1)
                morphology = morphologies[morphology.children[0]];
            else
            {
                primitives[morphology.branch].push_back(SpherePtr(
                    new Sphere(morphology.branch, va,
                        morphology.radius*_geometryParameters.getRadius()*morphology.children.size(),
                        static_cast<float>(morphology.frame))));
                bounds.merge(va);
                carryon = false;
            }
        }
        ++itb;
    }
    return true;
}
#else
bool MorphologyLoader::_importH5File(
        const std::string &, int, const Vector3f&, PrimitivesCollection&, Boxf&)
{
    BRAYNS_ERROR << "BBPSDK is neededstring to import H5 files" << std::endl;
    return false;
}
#endif

bool MorphologyLoader::_importSWCFile(
        const std::string &filename,
        const int morphologyIndex,
        const Vector3f& position,
        PrimitivesMap& primitives,
        Boxf& bounds)
{
    Morphologies morphologies;

    // DAT file
    std::map<int,int> mapIdTime;
    std::string datFilename(filename);

    size_t pos = datFilename.find( ".swc" );
    if ( pos != std::string::npos ) {
        datFilename.replace( pos, 4, ".dat");
    }

    std::ifstream datFile(datFilename);
    if(!datFile.is_open())
    {
        while(datFile.good())
        {
            std::string line;
            std::getline(datFile, line);
            if (line[0] == '#') continue;

            std::istringstream iss(line);
            int id, t;
            iss >> id >> t;
            mapIdTime[id] = t;
        }
        datFile.close();
    }

    // SWC file
    std::ifstream swcFile(filename);
    if(!datFile.is_open())
        return false;
    else
    {
        while(swcFile.good())
        {
            std::string line;
            std::getline(swcFile, line);
            if (line[0] == '#') continue;

            std::istringstream iss(line);
            int idx;
            Morphology morphology;
            iss >>
               idx >>
               morphology.branch >>
               morphology.x >> morphology.z >> morphology.y >>
               morphology.radius >>
               morphology.parent;

            morphology.x += position[0];
            morphology.y += position[1];
            morphology.z += position[2];
            morphology.id = idx;
            morphology.branch =
                ( _geometryParameters.getColorScheme() ==  CS_NEURON_BY_ID ?
                morphologyIndex : 0 );
            morphology.frame = mapIdTime[idx];
            morphology.used = false;

            if(morphology.parent > 0)
                morphologies[morphology.parent].children.push_back(idx);

            if( morphologies.find(idx) != morphologies.end() )
            {
                Morphology& m = morphologies[idx];
                m.x = morphology.x;
                m.y = morphology.y;
                m.z = morphology.z;
                m.id = morphology.id;
                m.branch = morphology.branch;
                m.frame = morphology.frame;
                m.used = morphology.used;
                m.parent = morphology.parent;
                m.radius = morphology.radius;
            }
            else
                morphologies[idx] = morphology;
        }
        swcFile.close();
    }

    Morphologies::iterator it=morphologies.begin();
    while( it!=morphologies.end())
    {
        Morphology& morphology = (*it).second;
        Vector3f morphologyPosition(morphology.x,morphology.y,morphology.z);

        if (morphology.parent == -1) // root soma, just one sphere
        {
            primitives[morphology.branch].push_back(SpherePtr(
                new Sphere(morphology.branch,
                   morphologyPosition,
                   morphology.radius*_geometryParameters.getRadius(),
                   static_cast<float>(morphology.frame))));
            bounds.merge(morphologyPosition);
            morphology.used = true;
        }
        else
        {
            Morphology& parentMorphology = morphologies[morphology.parent];
            if( parentMorphology.parent!=-1 )
            {
                Vector3f parentPosition(parentMorphology.x,parentMorphology.y,parentMorphology.z);
                Vector3f up = parentPosition - morphologyPosition;
                Vector3f v = morphologyPosition + up;

                primitives[morphology.branch].push_back(SpherePtr(
                    new Sphere(morphology.branch,
                        morphologyPosition,
                        morphology.radius*_geometryParameters.getRadius(),
                        static_cast<float>(morphology.frame))));

                primitives[morphology.branch].push_back(CylinderPtr(
                    new Cylinder(morphology.branch,
                        morphologyPosition, v,
                        morphology.radius*_geometryParameters.getRadius(),
                        static_cast<float>(morphology.frame))));

                bounds.merge(morphologyPosition);
                bounds.merge(v);
            }
        }
        ++it;
    }
    return true;
}

}
