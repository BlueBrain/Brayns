/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
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
    PrimitivesMap& primitives,
    Boxf& bounds)
{
    return _importMorphology(
        uri, morphologyIndex, Matrix4f::IDENTITY, primitives, bounds );
}

bool MorphologyLoader::_importMorphology(
    const servus::URI& source,
    const size_t morphologyIndex,
    const Matrix4f& transformation,
    PrimitivesMap& primitives,
    Boxf& bounds)
{
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

        if( morphologySectionTypes & MST_SOMA )
        {
            // Soma
            const brain::neuron::Soma& soma = morphology.getSoma();
            const size_t material =
                _material( morphologyIndex, brain::SECTION_SOMA );
            const Vector3f& center = soma.getCentroid() + translation;

            const float radius = ( _geometryParameters.getRadius() < 0.f ?
                - _geometryParameters.getRadius() :
                soma.getMeanRadius() * _geometryParameters.getRadius() );
            primitives[material].push_back( SpherePtr(
                new Sphere( material, center, radius, 0.f )));
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
            const size_t step =
                ( _geometryParameters.getGeometryQuality() == GQ_FAST ) ?
                    samples.size()-1 : 1;
            const float distanceToSoma = section.getDistanceToSoma();
            const floats& distancesToSoma = section.getSampleDistancesToSoma();

            for( size_t i = step; i < samples.size(); i += step )
            {
                Vector4f sample =  samples[i];
                const float radius = (_geometryParameters.getRadius() < 0.f ?
                    -_geometryParameters.getRadius() :
                    sample.w() * _geometryParameters.getRadius( ));
                const float previousRadius = (_geometryParameters.getRadius() < 0.f ?
                    -_geometryParameters.getRadius() :
                    previousSample.w() * _geometryParameters.getRadius( ));
                const float distance =
                    distanceToSoma + distancesToSoma[i];

                Vector3f position( sample.x(), sample.y(), sample.z());
                position += translation;
                Vector3f target(
                    previousSample.x(), previousSample.y(), previousSample.z());
                target += translation;
                primitives[material].push_back( SpherePtr(
                    new Sphere( material, position,
                        std::max( radius, previousRadius ),
                        distance )));
                bounds.merge( position );

                if( position != target )
                {
                    primitives[material].push_back( CylinderPtr(
                        new Cylinder( material, position, target,
                            previousRadius, distance )));
                    bounds.merge( target );
                }

                previousSample = sample;
            }
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
    PrimitivesMap& primitives,
    Boxf& bounds)
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
    const brain::URIs& uris = circuit.getMorphologyURIs( gids );
    const Matrix4fs& transforms = circuit.getTransforms( gids );

#pragma omp parallel
    {
        PrimitivesMap private_primitives;
        #pragma omp for nowait
        for( size_t i = 0; i < uris.size(); ++i )
        {
            const auto& uri = uris[i];
            _importMorphology(
                uri, i, transforms[i], private_primitives, bounds );
        }
        #pragma omp critical
        for( const auto& p: private_primitives )
        {
            const size_t material = p.first;
            primitives[material].insert(
                primitives[material].end(),
                private_primitives[material].begin(),
                private_primitives[material].end());
        }
    }

    return true;
}
#else
bool MorphologyLoader::importMorphology(
    const servus::URI&, const int, PrimitivesMap& , Boxf& )
{
    BRAYNS_ERROR << "Brion is required to load morphologies" << std::endl;
    return false;
}

bool MorphologyLoader::importCircuit(
    const servus::URI&, const std::string&, PrimitivesMap&, Boxf& )
{
    BRAYNS_ERROR << "Brion is required to load morphologies" << std::endl;
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
        material = morphologyIndex % DEFAULT_NB_MATERIALS;
        break;
    case CS_NEURON_BY_SEGMENT_TYPE:
        material = sectionType % DEFAULT_NB_MATERIALS;
        break;
    default:
        material = 0;
    }
    return material;
}

}
