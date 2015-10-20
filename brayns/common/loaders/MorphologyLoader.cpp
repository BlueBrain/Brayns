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
#include <map>

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
    : geometryParameters_(geometryParameters)
{
}

#ifdef BRAYNS_USE_BBPSDK
void MorphologyLoader::importH5Morphologies(
        const std::string &filename,
        int morphologyIndex,
        const vec3f& position,
        Geometries& geometries,
        box3f& bounds)
{
    Morphologies morphologies;
    std::map<uint16_t, float> sectionsLengths;

    ospray::vec3f somaPosition(0.f,0.f,0.f);
    vec3f randomPosition(0.f,0.f,0.f);
    for( size_t c=0; c<geometryParameters_.getReplicas(); ++c )
    {
        bbp::Morphology_Reader reader;
        reader.open(filename.c_str());
        if( reader.is_open() )
        {
            bbp::Morphologies bbpMorphologies;
            reader.read( bbpMorphologies );
            reader.close();

            bbp::Morphologies::iterator it =  bbpMorphologies.begin();
            while( it != bbpMorphologies.end() )
            {
                size_t frameId = 0;
                int material = geometryParameters_.getColored() ?
                            (morphologyIndex%100) : 0;
                bbp::Morphology m = (*it);

                size_t count_sections = 0;
                bbp::Sections sections = m.sections();
                BOOST_FOREACH( bbp::Section section, sections )
                {
                    // Soma
                    bbp::Soma soma = m.soma();
                    const bbp::Vector3f pos = soma.position();
                    vec3f v = vec3f(pos[0],pos[1],pos[2]);
                    v += randomPosition+position;
                    if( count_sections == 0 )
                    {
                        somaPosition = v;
                        geometries.push_back(
                                    (brayns::Geometry){
                                        gt_sphere, v, v,
                                        0.5f*soma.max_radius()*geometryParameters_.getRadius(), 0,
                                        static_cast<float>(frameId), material});
                        bounds.extend(v);
                    }
                    vec3f va = v;

                    size_t count_segments = 0;
                    const bbp::Segments& segments = section.segments();
                    BOOST_FOREACH( bbp::Segment segment, segments )
                    {
                        const bbp::Vector3f b = segment.end().center();
                        bbp::Cross_Section cs = segment.cross_section(1.0);
                        vec3f vb(b[0],b[1],b[2]);
                        vb += randomPosition+position;

                        float r = cs.radius()*geometryParameters_.getRadius();
                        frameId = length(vb-v);

                        // Branches
                        geometries.push_back(
                                    (brayns::Geometry){
                                        gt_sphere, vb, vb, r, 0,
                                        static_cast<float>(frameId), material});
                        geometries.push_back(
                                    (brayns::Geometry){
                                        gt_cylinder, va, vb, r, 0,
                                        static_cast<float>(frameId), material});
                        bounds.extend(va);
                        bounds.extend(vb);

                        va = vb;
                        ++count_segments;
                    }
                    ++count_sections;
                }
                ++morphologyIndex;
                ++it;
            }
        }
        randomPosition.x = (rand()%1000-500);
        randomPosition.y = (rand()%1000-500);
        randomPosition.z = (rand()%1000-500);
    }
    Branches branches;

    Morphologies::iterator itm=morphologies.begin();
    while( itm!=morphologies.end())
    {
        Morphology& a = (*itm).second;
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

    Branches::iterator itb=branches.begin();
    int branch(0);
    while( itb!=branches.end())
    {
        int segment(0);
        vec3fa vb;
        Branch b = (*itb);
        Morphology& m = morphologies[b.segments[0]];
        vec3f begin;
        bool carryon(true);
        while( carryon )
        {
            vec3fa va(m.x,m.y,m.z, m.radius * geometryParameters_.getRadius());

            if( segment>0 )
            {
                vec3f up = vb - va;
                geometries.push_back(
                            (brayns::Geometry){
                                gt_sphere, va, va+up,
                                m.radius * geometryParameters_.getRadius(), 0,
                                float(m.frame), m.branch});
                bounds.extend(va);
            }
            vb = va;
            ++segment;

            if(m.children.size()==1)
            {
                m = morphologies[m.children[0]];
            }
            else
            {
                geometries.push_back(
                            (brayns::Geometry){
                                gt_sphere, va, va,
                                m.radius * geometryParameters_.getRadius() *
                                (m.children.size()), 0,
                                float(m.frame), m.branch});
                bounds.extend(va);
                carryon = false;
            }
        }
        ++itb;
        ++branch;
        branch = branch%100;
    }
}
#endif

void MorphologyLoader::importSWCMorphologies(
        const std::string &filename,
        const int /*morphologyIndex*/,
        const vec3f& position,
        Geometries& geometries,
        box3f& bounds)
{
    Morphologies morphologies;

    // DAT file
    std::map<int,int> mapIdTime;
    std::string f(filename);

    size_t pos = f.find( ".swc" );
    if ( pos != std::string::npos ) {
        f.replace( pos, 4, ".dat");
    }

    FILE *file = fopen(f.c_str(),"r");
    if( file )
    {
        for (char line[10000]; fgets(line,10000,file) && !feof(file); )
        {
            if (line[0] == '#') continue;
            int id, t;
            sscanf(line,"%i %i", &id, &t);
            mapIdTime[id] = t;
        }
    }

    file = fopen(filename.c_str(),"r");
    Assert(file);
    for (char line[10000]; fgets(line,10000,file) && !feof(file); )
    {
        if (line[0] == '#') continue;
        int idx;
        Morphology morphology;
        sscanf(line,
               "%i %i %f %f %f %f %i\n",
               &idx,
               &morphology.branch,
               &morphology.x, &morphology.z, &morphology.y,
               &morphology.radius,
               &morphology.parent);

        morphology.x += position.x;
        morphology.y += position.y;
        morphology.z += position.z;
        morphology.id = idx;
        if( !geometryParameters_.getColored() )
            morphology.branch = 0;
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
        {
            morphologies[idx] = morphology;
        }
    }
    fclose(file);

    ospray::vec3f randomPosition(0.f,0.f,0.f);
    for( size_t c(0); c<geometryParameters_.getReplicas(); ++c)
    {
        Morphologies::iterator it=morphologies.begin();
        while( it!=morphologies.end())
        {
            Morphology& morphology = (*it).second;
            vec3f morphologyPosition(morphology.x,morphology.y,morphology.z);
            morphologyPosition += randomPosition;

            if (morphology.parent == -1) // root soma, just one sphere
            {
                geometries.push_back(
                            (brayns::Geometry){
                                gt_sphere, morphologyPosition, morphologyPosition,
                                morphology.radius*geometryParameters_.getRadius(), 0.f,
                                static_cast<float>(morphology.frame),
                                morphology.branch});
                bounds.extend(morphologyPosition);
                morphology.used = true;
            }
            else
            {
                Morphology& parentMorphology = morphologies[morphology.parent];
                if( parentMorphology.parent!=-1 )
                {
                    vec3f parentPosition(parentMorphology.x,parentMorphology.y,parentMorphology.z);
                    parentPosition += randomPosition;
                    vec3f up = parentPosition - morphologyPosition;
                    vec3f v = morphologyPosition + up*4.f;
                    geometries.push_back(
                                (brayns::Geometry){
                                    gt_sphere, morphologyPosition, v,
                                    parentMorphology.radius * geometryParameters_.getRadius(), 0.f,
                                    static_cast<float>(parentMorphology.frame),
                                    parentMorphology.branch});
                }
            }
            ++it;
        }

        if( geometryParameters_.getReplicas()>1 )
        {
            // Randomize position for following morphologies
            randomPosition.x = (rand()%1000-500);
            randomPosition.y = (rand()%1000-500);
            randomPosition.z = (rand()%1000-500);
        }
    }
}

void MorphologyLoader::importMorphologiesToStreamLines(
        const std::string &filename,
        const int morphologyIndex,
        const int precision,
        StreamLinesCollection streamlines,
        box3f& bounds )
{

    Morphologies morphologies;

    FILE* file = fopen(filename.c_str(),"r");
    Assert(file);
    for (char line[10000]; fgets(line,10000,file) && !feof(file); )
    {
        if (line[0] == '#') continue;
        int idx;
        Morphology morphology;
        sscanf(line,
               "%i %i %f %f %f %f %i\n",
               &idx,
               &morphology.branch,
               &morphology.x, &morphology.z, &morphology.y,
               &morphology.radius,
               &morphology.parent);

        morphology.id = idx;
        morphology.branch = morphologyIndex;
        morphology.frame = 0;
        morphology.used = false;

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
        {
            morphologies[idx] = morphology;
        }
    }
    fclose(file);

    Branches branches;
    Morphologies::iterator itm=morphologies.begin();
    while( itm!=morphologies.end())
    {
        Morphology& a = (*itm).second;
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
    BRAYNS_DEBUG << branches.size() << " branches" << std::endl;

    Branches::iterator itb=branches.begin();
    while( itb!=branches.end() )
    {
        int c(0);
        Branch b = (*itb);
        Morphology& m = morphologies[b.segments[0]];

        bool carryon(true);
        while( carryon )
        {
            ospray::vec3fa v(m.x,m.y,m.z,1.f);
            if(c%precision==0)
            {
                if(c>0)
                {
                    streamlines[morphologyIndex].index.push_back(
                                streamlines[morphologyIndex].vertex.size()-1);
                }
                streamlines[morphologyIndex].vertex.push_back(v);
                streamlines[morphologyIndex].radius =
                        m.radius*geometryParameters_.getRadius();
                bounds.extend(v);
            }
            ++c;

            if(m.children.size()==1)
                m = morphologies[m.children[0]];
            else
                carryon = false;
        }
        ++itb;
    }
}

}
