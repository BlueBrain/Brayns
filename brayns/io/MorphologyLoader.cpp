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
#include <brayns/common/scene/Scene.h>
#include <brayns/common/simulation/CircuitSimulationHandler.h>

#include <algorithm>
#include <fstream>

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
    const SimulationInformation* simulationInformation,
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
        brain::neuron::SectionTypes sectionTypes;

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
            sectionTypes.push_back( brain::neuron::SectionType::soma );
        if( morphologySectionTypes & MST_AXON )
            sectionTypes.push_back( brain::neuron::SectionType::axon );
        if( morphologySectionTypes & MST_DENDRITE )
            sectionTypes.push_back( brain::neuron::SectionType::dendrite );
        if( morphologySectionTypes & MST_APICAL_DENDRITE )
            sectionTypes.push_back( brain::neuron::SectionType::apicalDendrite );

        const brain::neuron::Sections& sections =
            morphology.getSections( sectionTypes );

        size_t sectionId = 0;

        float offset = 0.f;
        if( simulationInformation )
            offset = (*simulationInformation->compartmentOffsets)[sectionId];
        else
            if( simulationOffset != 0 )
                offset = simulationOffset;

        if( morphologySectionTypes & MST_SOMA )
        {
            // Soma
            const brain::neuron::Soma& soma = morphology.getSoma();
            const size_t material =
                _material( morphologyIndex,
                           size_t( brain::neuron::SectionType::soma ));
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
                _material( morphologyIndex, size_t( section.getType( )));
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
            if( simulationInformation )
            {
                const auto& counts = *simulationInformation->compartmentCounts;
                // Number of compartments usually differs from number of samples
                if( samples.size() != 0 && counts[sectionId] > 1 )
                    segmentStep = counts[sectionId] / float(samples.size());
            }

            bool done = false;
            for( size_t i = step; !done && i < samples.size() + step; i += step )
            {
                if( i >= samples.size( ))
                {
                    i = samples.size() - 1;
                    done = true;
                }

                const float distance = distanceToSoma + distancesToSoma[i];

                maxDistanceToSoma = std::max(maxDistanceToSoma, distance);

                if( simulationInformation )
                    offset = (*simulationInformation->compartmentOffsets)[sectionId] + float(i)*segmentStep;
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
    size_t progress = 0;
    #pragma omp parallel
    {
        PrimitivesMap private_primitives;
        #pragma omp for nowait
        for( size_t i = 0; i < uris.size(); ++i )
        {
            const auto& uri = uris[i];
            float maxDistanceToSoma = 0.f;
            if( _importMorphology(
                uri, i, transforms[i], 0,
                private_primitives, scene.getWorldBounds(),
                simulationOffset, maxDistanceToSoma))
            {
                morphologyOffsets[simulatedCells] = maxDistanceToSoma;
                simulationOffset += maxDistanceToSoma;
            }

            BRAYNS_PROGRESS( progress, uris.size() );
            #pragma omp atomic
            ++progress;
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

    size_t progress = 0;
    #pragma omp parallel
    {
        PrimitivesMap private_primitives;
        #pragma omp for nowait
        for( size_t i = 0; i < cr_uris.size(); ++i )
        {
            const auto& uri = cr_uris[i];
            const SimulationInformation simulationInformation =
            {
                &compartmentCounts[i],
                &compartmentOffsets[i]
            };

            float maxDistanceToSoma;
            _importMorphology(
                uri, i, transforms[i], &simulationInformation,
                private_primitives, scene.getWorldBounds(),
                0, maxDistanceToSoma);

            BRAYNS_PROGRESS( progress, cr_uris.size() );
            #pragma omp atomic
            ++progress;
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

        progress = 0;
        #pragma omp parallel
        {
            PrimitivesMap private_primitives;
            #pragma omp for nowait
            for( size_t i = 0; i < nonSimulatedCells; ++i )
            {
                float maxDistanceToSoma;
                const auto& uri = allUris[i];

                _importMorphology(
                    uri, i, allTransforms[i], 0,
                    private_primitives, scene.getWorldBounds(),
                    0, maxDistanceToSoma);

                BRAYNS_PROGRESS( progress, allUris.size() );
                #pragma omp atomic
                ++progress;
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

bool MorphologyLoader::importSimulationData(
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
        return false;
    }

    // Load simulation information from compartment reports
    brion::CompartmentReport compartmentReport(
        brion::URI( bc.getReportSource( report ).getPath( )), brion::MODE_READ, gids );

    CircuitSimulationHandlerPtr simulationHandler( new CircuitSimulationHandler( ));
    scene.setSimulationHandler( simulationHandler );
    const std::string& cacheFile = _geometryParameters.getSimulationCacheFile();
    if( simulationHandler->attachSimulationToCacheFile( cacheFile ))
        // Cache already exists, no need to create it.
        return true;

    BRAYNS_INFO << "Cache file does not exist, creating it" << std::endl;
    std::ofstream file( cacheFile, std::ios::out | std::ios::binary );

    if( !file.is_open() )
    {
        BRAYNS_ERROR << "Failed to create cache file" << std::endl;
        return false;
    }

    // Load simulation information from compartment reports
    const float start = compartmentReport.getStartTime();
    const float end = compartmentReport.getEndTime();
    const float step = compartmentReport.getTimestep();

    const float firstFrame = std::max( start, _geometryParameters.getStartSimulationTime() );
    const float lastFrame = std::min( end, _geometryParameters.getEndSimulationTime() );
    const uint64_t frameSize = compartmentReport.getFrameSize();

    const uint64_t nbFrames = ( lastFrame - firstFrame ) / step;

    BRAYNS_INFO << "Loading values from compartment report and saving them to cache" << std::endl;

    // Write header
    simulationHandler->setNbFrames( nbFrames );
    simulationHandler->setFrameSize( frameSize );
    simulationHandler->writeHeader( file );

    // Write body
    for( uint64_t frame = 0; frame < nbFrames; ++frame )
    {
        BRAYNS_PROGRESS( frame, nbFrames );
        const float frameTime = firstFrame + step * frame;
        const brion::floatsPtr& valuesPtr = compartmentReport.loadFrame( frameTime );
        const floats& values = *valuesPtr;
        simulationHandler->writeFrame( file, values );
    }
    file.close();

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Cache file successfully created" << std::endl;
    BRAYNS_INFO << "Number of frames: " << nbFrames << std::endl;
    BRAYNS_INFO << "Frame size      : " << frameSize << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;
    return true;
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

bool MorphologyLoader::importSimulationData(
    const servus::URI&, const std::string&, const std::string&, Scene& )
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
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
