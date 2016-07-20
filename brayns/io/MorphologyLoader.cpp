/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/scene/Scene.h>
#include <algorithm>

#ifdef BRAYNS_USE_BRION
#  include <brain/brain.h>
#  include <brion/brion.h>
#endif

namespace brayns
{

MorphologyLoader::MorphologyLoader(
        const GeometryParameters& geometryParameters )
    : _geometryParameters(geometryParameters)
{
}

#ifdef BRAYNS_USE_BRION
bool MorphologyLoader::importMorphology(
    const servus::URI& uri,
    const int morphologyIndex,
    Scene& scene)
{
    float maxDistanceToSoma;
    return _importMorphology(
        uri, morphologyIndex, Matrix4f(),
        0, scene.getPrimitives(),
        scene.getWorldBounds(), 0, maxDistanceToSoma);
}

bool MorphologyLoader::_importMorphology(
    const servus::URI& source,
    const size_t morphologyIndex,
    const Matrix4f& transformation,
    const SimulationData* simulationData,
    PrimitivesMap& primitives,
    Boxf& bounds,
    const size_t simulationOffset,
    float& maxDistanceToSoma)
{
    maxDistanceToSoma = 0.f;
    try
    {
        Vector3f translation = { 0.f, 0.f, 0.f };

        brain::neuron::Morphology morphology( source, transformation );
        brain::SectionTypes sectionTypes;

        const MorphologyLayout& layout =
            _geometryParameters.getMorphologyLayout();

        if( layout.type != ML_NONE )
        {
            Boxf morphologyAABB;
            const brain::Vector4fs& points = morphology.getPoints();
            for( Vector4f point: points )
            {
                const Vector3f p = { point.x(), point.y(), point.z() };
                morphologyAABB.merge( p );
            }

            const Vector3f positionInGrid =
            {
                -layout.horizontalSpacing * static_cast< float >(
                    morphologyIndex % layout.nbColumns ),
                -layout.verticalSpacing * static_cast< float >(
                    morphologyIndex / layout.nbColumns),
                0.f
            };
            translation = positionInGrid - morphologyAABB.getCenter();
        }

        const size_t morphologySectionTypes =
            _geometryParameters.getMorphologySectionTypes();
        if( morphologySectionTypes & MST_SOMA )
            sectionTypes.push_back( brain::SECTION_SOMA );
        if( morphologySectionTypes & MST_AXON )
            sectionTypes.push_back( brain::SECTION_AXON );
        if( morphologySectionTypes & MST_DENDRITE )
            sectionTypes.push_back( brain::SECTION_DENDRITE );
        if( morphologySectionTypes & MST_APICAL_DENDRITE )
            sectionTypes.push_back( brain::SECTION_APICAL_DENDRITE );

        const brain::neuron::Sections& sections =
            morphology.getSections( sectionTypes );

        size_t sectionId = 0;

        float offset = -1.f;
        if( simulationData )
            offset = (*simulationData->compartmentOffsets)[sectionId];
        else
            if( simulationOffset != 0 )
                offset = simulationOffset;

        if( morphologySectionTypes & MST_SOMA )
        {
            // Soma
            const brain::neuron::Soma& soma = morphology.getSoma();
            const size_t material =
                _material( morphologyIndex, brain::SECTION_SOMA );
            const Vector3f& center = soma.getCentroid() + translation;

            const float radius =
                ( _geometryParameters.getRadiusCorrection() != 0.f ?
                _geometryParameters.getRadiusCorrection() :
                soma.getMeanRadius() *
                    _geometryParameters.getRadiusMultiplier() );
            primitives[material].push_back( SpherePtr(
                new Sphere( material, center, radius, 0.f, offset )));
            bounds.merge( center );
        }

        // Dendrites and axon
        for( const auto& section: sections )
        {
            const size_t material =
                _material( morphologyIndex, section.getType( ));
            const Vector4fs& samples = section.getSamples();
            if( samples.size() == 0 )
                continue;

            Vector4f previousSample = samples[0];
            size_t step = 1;
            switch( _geometryParameters.getGeometryQuality() )
            {
                case GQ_FAST:
                    step = samples.size()-1;
                    break;
                case GQ_QUALITY:
                    step = samples.size()/2;
                    step = ( step == 0 ) ? 1 : step;
                    break;
                default:
                    step = 1;
            }

            const float distanceToSoma = section.getDistanceToSoma();
            const floats& distancesToSoma = section.getSampleDistancesToSoma();

            float segmentStep = 0.f;
            if( simulationData )
            {
                // Number of compartments usually differs from number of samples
                if( samples.size() != 0 && (*simulationData->compartmentCounts)[sectionId] > 1 )
                    segmentStep =
                        float((*simulationData->compartmentCounts)[sectionId]) /
                        float(samples.size());
            }

            bool done = false;
            for( size_t i = step; !done && i < samples.size() + step; i += step )
            {
                if( i>=samples.size() )
                {
                    i = samples.size()-1;
                    done = true;
                }

                const float distance =
                    distanceToSoma + distancesToSoma[i];

                maxDistanceToSoma = std::max(maxDistanceToSoma, distance);

                if( simulationData )
                {
                    offset = (*simulationData->compartmentOffsets)[sectionId] + float(i)*segmentStep;
                    if( offset>1e4f ) // TODO: Why am I getting weird values??
                        offset = -1.f;
                }
                else
                    if( simulationOffset != 0 )
                        offset = simulationOffset + distance;

                Vector4f sample =  samples[i];
                const float previousRadius =
                    (_geometryParameters.getRadiusCorrection() != 0.f ?
                    _geometryParameters.getRadiusCorrection() :
                    samples[ i - step ].w() * 0.5f *
                        _geometryParameters.getRadiusMultiplier( ));

                Vector3f position( sample.x(), sample.y(), sample.z());
                position += translation;
                Vector3f target(
                    previousSample.x(), previousSample.y(), previousSample.z());
                target += translation;
                const float radius =
                    (_geometryParameters.getRadiusCorrection() != 0.f ?
                    _geometryParameters.getRadiusCorrection() :
                    samples[ i ].w() * 0.5f *
                         _geometryParameters.getRadiusMultiplier( ));

                if( radius > 0.f )
                    primitives[material].push_back( SpherePtr(
                        new Sphere( material, position,
                            radius, distance, offset )));

                bounds.merge( position );
                if( position != target && radius > 0.f && previousRadius > 0.f )
                {
                    if( radius == previousRadius )
                        primitives[material].push_back( CylinderPtr(
                            new Cylinder( material, position, target,
                                radius, distance, offset )));
                    else
                        primitives[material].push_back( ConePtr(
                            new Cone( material, position, target,
                                radius, previousRadius, distance, offset )));
                    bounds.merge( target );
                }
                previousSample = sample;
            }
            ++sectionId;
        }
    }
    catch( const std::runtime_error& e )
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return false;
    }
    return true;
}

bool MorphologyLoader::importCircuit(
    const servus::URI& circuitConfig,
    const std::string& target,
    Scene& scene)
{
    const std::string& filename = circuitConfig.getPath();
    const brion::BlueConfig bc( filename );
    const brain::Circuit circuit( bc );
    const brain::GIDSet& gids =
        ( target.empty() ? circuit.getGIDs() : circuit.getGIDs( target ));
    if( gids.empty() )
    {
        BRAYNS_ERROR << "Circuit does not contain any cells" << std::endl;
        return false;
    }
    const Matrix4fs& transforms = circuit.getTransforms( gids );

    const brain::URIs& uris = circuit.getMorphologyURIs( gids );

    BRAYNS_INFO << "Loading " << uris.size() << " cells" << std::endl;

    std::map< size_t, float > morphologyOffsets;

    size_t simulationOffset = 1;
    size_t simulatedCells = 0;
    #pragma omp parallel
    {
        PrimitivesMap private_primitives;
        #pragma omp for nowait
        for( size_t i = 0; i <uris.size(); ++i )
        {
            const auto& uri = uris[i];
            BRAYNS_INFO << "[" << i << "/" << uris.size() << "] "
                        << uri << std::endl;
            float maxDistanceToSoma = 0.f;
            if( _importMorphology(
                uri, i, transforms[i], 0,
                private_primitives, scene.getWorldBounds(),
                simulationOffset, maxDistanceToSoma))
            {
                morphologyOffsets[simulatedCells] = maxDistanceToSoma;
                simulationOffset += maxDistanceToSoma;
            }
        }
        #pragma omp critical
        for( const auto& p: private_primitives )
        {
            const size_t material = p.first;
            scene.getPrimitives()[material].insert(
                scene.getPrimitives()[material].end(),
                private_primitives[material].begin(),
                private_primitives[material].end());
        }
    }

    return true;
}

bool MorphologyLoader::importCircuit(
    const servus::URI& circuitConfig,
    const std::string& target,
    const std::string& report,
    Scene& scene)
{
    const std::string& filename = circuitConfig.getPath();
    const brion::BlueConfig bc( filename );
    const brain::Circuit circuit( bc );
    const brain::GIDSet& gids =
        ( target.empty() ? circuit.getGIDs() : circuit.getGIDs( target ));
    if( gids.empty() )
    {
        BRAYNS_ERROR << "Circuit does not contain any cells" << std::endl;
        return false;
    }
    const Matrix4fs& transforms = circuit.getTransforms( gids );

    const brain::URIs& uris = circuit.getMorphologyURIs( gids );

    // Load simulation information from compartment reports
    const brion::CompartmentReport compartmentReport(
        brion::URI( bc.getReportSource( report ).getPath( )), brion::MODE_READ,
                    gids );

    const brion::CompartmentCounts& compartmentCounts =
        compartmentReport.getCompartmentCounts();

    const brion::SectionOffsets& compartmentOffsets =
        compartmentReport.getOffsets();

    brain::URIs cr_uris;
    const brain::GIDSet& cr_gids = compartmentReport.getGIDs();

    BRAYNS_INFO << "Loading " << cr_gids.size()
                << " simulated cells" << std::endl;
    for( const auto cr_gid: cr_gids)
    {
        auto it = std::find( gids.begin(), gids.end(), cr_gid );
        auto index = std::distance( gids.begin(), it );
        cr_uris.push_back( uris[ index ] );
    }

    #pragma omp parallel
    {
        PrimitivesMap private_primitives;
        #pragma omp for nowait
        for( size_t i = 0; i < cr_uris.size(); ++i )
        {
            const auto& uri = cr_uris[i];
            BRAYNS_DEBUG << "[" << i << "/" << uris.size() << "] "
                         << uri << std::endl;
            const SimulationData simulationData = {
                &compartmentCounts[i],
                &compartmentOffsets[i]
            };

            float maxDistanceToSoma;
            _importMorphology(
                uri, i, transforms[i], &simulationData,
                private_primitives, scene.getWorldBounds(),
                0, maxDistanceToSoma);
        }
        #pragma omp critical
        for( const auto& p: private_primitives )
        {
            const size_t material = p.first;
            scene.getPrimitives()[material].insert(
                scene.getPrimitives()[material].end(),
                private_primitives[material].begin(),
                private_primitives[material].end());
        }
    }

    size_t nonSimulatedCells =
        _geometryParameters.getNonSimulatedCells();
    if( nonSimulatedCells != 0 )
    {
        // Non simulated cells
        const brain::GIDSet& allGids = circuit.getGIDs();
        const brain::URIs& allUris = circuit.getMorphologyURIs( allGids );
        const Matrix4fs& allTransforms = circuit.getTransforms( allGids );

        cr_uris.clear();
        size_t index = 0;
        for( const auto gid: allGids)
        {
            auto it = std::find( cr_gids.begin(), cr_gids.end(), gid );
            if( it == cr_gids.end() )
                cr_uris.push_back( allUris[ index ] );
            ++index;
        }

        if( cr_uris.size() < nonSimulatedCells )
            nonSimulatedCells = cr_uris.size();

        BRAYNS_INFO << "Loading " << nonSimulatedCells
                    << " non-simulated cells" << std::endl;

#pragma omp parallel
        {
            PrimitivesMap private_primitives;
            #pragma omp for nowait
            for( size_t i = 0; i < nonSimulatedCells; ++i )
            {
                float maxDistanceToSoma;
                const auto& uri = allUris[i];
                BRAYNS_INFO << "[" << i << "/" << nonSimulatedCells << "] "
                            << uri << std::endl;


                _importMorphology(
                    uri, i, allTransforms[i], 0,
                    private_primitives, scene.getWorldBounds(),
                    0, maxDistanceToSoma);
            }
            #pragma omp critical
            for( const auto& p: private_primitives )
            {
                const size_t material = p.first;
                scene.getPrimitives()[material].insert(
                    scene.getPrimitives()[material].end(),
                    private_primitives[material].begin(),
                    private_primitives[material].end());
            }
        }
    }
    return true;
}

size_t MorphologyLoader::importSimulationIntoTexture(
    const servus::URI& circuitConfig,
    const std::string& target,
    const std::string& report,
    Scene& scene )
{
    const std::string& filename = circuitConfig.getPath();
    const brion::BlueConfig bc( filename );
    const brain::Circuit circuit( bc );
    const brain::GIDSet& gids =
        ( target.empty() ? circuit.getGIDs() : circuit.getGIDs( target ));
    if( gids.empty( ))
    {
        BRAYNS_ERROR << "Circuit does not contain any cells" << std::endl;
        return 0;
    }

    // Load simulation information from compartment reports
    brion::CompartmentReport compartmentReport(
        brion::URI( bc.getReportSource( report ).getPath( )), brion::MODE_READ,
                gids );

    const float start = compartmentReport.getStartTime();
    const float end = compartmentReport.getEndTime();
    const float step = compartmentReport.getTimestep();
    const uint64_t nbChannels = 3;

    const uint64_t totalNbFrames = (end-start) / step;

    uint64_t nbFrames = totalNbFrames;

    const uint64_t firstFrame =
        _geometryParameters.getFirstSimulationFrame();

    uint64_t lastFrame =
        _geometryParameters.getLastSimulationFrame();

    if( lastFrame == 0 )
        lastFrame = totalNbFrames;

    if( lastFrame > totalNbFrames )
        lastFrame = totalNbFrames;

    const uint64_t frameSize =
        compartmentReport.getFrameSize();

    nbFrames = lastFrame - firstFrame;

    Texture2DPtr texture( new Texture2D );
    texture->setType( TT_DIFFUSE );
    texture->setNbChannels( nbChannels );
    texture->setDepth( 1 );
    texture->setWidth( frameSize );

    const uint64_t maxTextureSize = 2147483648; // 2GB

    if( nbFrames * frameSize * nbChannels >= maxTextureSize )
    {
        // OSPRay is currently limited to 2GB textures, we therefore limit the
        // number of frames accordingly.
        // TODO: Create one texture per frame? Discuss with Intel first!
        nbFrames = maxTextureSize / (frameSize * nbChannels);

        BRAYNS_WARN << "Simulation is too big to fit in one texture (" <<
            nbFrames * frameSize << " bytes). " <<
            nbFrames << " out of " << totalNbFrames <<
            " frames will be loaded" << std::endl;
    }

    texture->setHeight( nbFrames );

    size_t totalSize = 0;
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    for( size_t frame = firstFrame; frame < firstFrame + nbFrames; ++frame )
    {
        const brion::floatsPtr& valuesPtr =
            compartmentReport.loadFrame( start + step * frame );
        const floats& values = *valuesPtr;

        for(size_t i = 0; i < values.size(); ++i )
        {
            min = std::min( min, values[i] );
            max = std::max( max, values[i] );
        }
        totalSize += values.size();
    }

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Frames loaded   : [" << firstFrame << " - "
                << lastFrame << "] " << nbFrames << "/"
                << totalNbFrames << std::endl;
    BRAYNS_INFO << "Simulation range: [" <<
        min << " : " << max << "]" << std::endl;
    BRAYNS_INFO << "Texture size    : " <<
        texture->getWidth() << "x" << texture->getHeight() << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;

    const float colorStep = 255.f / float( max - min );
    std::vector< unsigned char > data;
    data.reserve( totalSize * nbChannels );
    for( size_t frame = firstFrame; frame < firstFrame + nbFrames; ++frame )
    {
        const brion::floatsPtr& valuesPtr =
            compartmentReport.loadFrame( start + step * frame );
        const floats& values = *valuesPtr;

        for( size_t i = 0; i < values.size(); ++i )
        {
            const size_t value = float( values[i] - min ) * colorStep;
            data.push_back( value );
            data.push_back( value );
            data.push_back( value );
        }
    }
    texture->setRawData( data.data(), totalSize * nbChannels );
    scene.getTextures()[TEXTURE_NAME_SIMULATION] = texture;
    return nbFrames;
}

#else

bool MorphologyLoader::importMorphology(
    const servus::URI&, const int, Scene& )
{
    BRAYNS_ERROR << "Brion is required to load morphologies" << std::endl;
    return false;
}

bool MorphologyLoader::importCircuit(
    const servus::URI&, const std::string&, Scene& )
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

bool MorphologyLoader::importCircuit(
    const servus::URI&, const std::string&, const std::string&, Scene& )
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

size_t MorphologyLoader::importSimulationIntoTexture(
    const servus::URI&,
    const std::string&,
    const std::string&,
    Scene& )
{
    BRAYNS_ERROR << "Brion is required to load simulations" << std::endl;
    return 0;
}
#endif

size_t MorphologyLoader::_material(
    const size_t morphologyIndex,
    const size_t sectionType )
{
    size_t material;
    switch( _geometryParameters.getColorScheme() )
    {
    case CS_NEURON_BY_ID:
        material = morphologyIndex % (NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS);
        break;
    case CS_NEURON_BY_SEGMENT_TYPE:
        material = sectionType % (NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS);
        break;
    default:
        material = 0;
    }
    return material;
}


}
