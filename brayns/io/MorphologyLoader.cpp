/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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
#include "circuitLoaderCommon.h"

#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/io/algorithms/MetaballsGenerator.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <boost/filesystem.hpp>

namespace
{
// needs to be the same in SimulationRenderer.ispc
const float INDEX_MAGIC = 1e6;
}

namespace brayns
{
class MorphologyLoader::Impl
{
public:
    Impl(const GeometryParameters& geometryParameters)
        : _geometryParameters(geometryParameters)
    {
    }

    /**
     * @brief importMorphology imports a single morphology from a specified URI
     * @param uri URI of the morphology
     * @param index Index of the morphology
     * @param defaultMaterialId Material to use
     * @param transformation Transformation to apply to the morphology
     * @param compartmentReport Compartment report to map to the morphology
     * @return True is the morphology was successfully imported, false otherwise
     */
    bool importMorphology(const servus::URI& source, Model& model,
                          const uint64_t index, const Matrix4f& transformation,
                          const size_t defaultMaterialId = NO_MATERIAL,
                          CompartmentReportPtr compartmentReport = nullptr)
    {
        ParallelModelContainer modelContainer(model.getSpheres(),
                                              model.getCylinders(),
                                              model.getCones(),
                                              model.getTrianglesMeshes(),
                                              model.getBounds());

        auto materialFunc = [
            defaultMaterialId,
            colorScheme = _geometryParameters.getColorScheme(), index
        ](auto sectionType)
        {
            if (defaultMaterialId != NO_MATERIAL)
                return defaultMaterialId;

            size_t materialId = 0;
            switch (colorScheme)
            {
            case ColorScheme::neuron_by_id:
                materialId = index;
                break;
            case ColorScheme::neuron_by_segment_type:
                switch (sectionType)
                {
                case brain::neuron::SectionType::soma:
                    materialId = 1;
                    break;
                case brain::neuron::SectionType::axon:
                    materialId = 2;
                    break;
                case brain::neuron::SectionType::dendrite:
                    materialId = 3;
                    break;
                case brain::neuron::SectionType::apicalDendrite:
                    materialId = 4;
                    break;
                default:
                    materialId = 0;
                    break;
                }
                break;
            default:
                materialId = 0;
            }
            return materialId;
        };

        const bool returnValue =
            importMorphology(source, index, materialFunc, transformation,
                             compartmentReport, modelContainer);
        model.createMissingMaterials();
        return returnValue;
    }

    bool importMorphology(const servus::URI& source, const uint64_t index,
                          MaterialFunc materialFunc,
                          const Matrix4f& transformation,
                          CompartmentReportPtr compartmentReport,
                          ParallelModelContainer& model)
    {
        bool returnValue = true;
        const size_t morphologySectionTypes =
            enumsToBitmask(_geometryParameters.getMorphologySectionTypes());
        if (morphologySectionTypes ==
            static_cast<size_t>(MorphologySectionType::soma))
            return _importMorphologyAsPoint(index, materialFunc, transformation,
                                            compartmentReport, model);
        else if (_geometryParameters.useRealisticSomas())
            returnValue = _createRealisticSoma(source, materialFunc,
                                               transformation, model);
        returnValue =
            returnValue &&
            _importMorphologyFromURI(source, index, materialFunc,
                                     transformation, compartmentReport, model);
        return returnValue;
    }

private:
    /**
     * @brief _getCorrectedRadius Modifies the radius of the geometry according
     * to --radius-multiplier and --radius-correction geometry parameters
     * @param radius Radius to be corrected
     * @return Corrected value of a radius according to geometry parameters
     */
    float _getCorrectedRadius(const float radius) const
    {
        return (_geometryParameters.getRadiusCorrection() != 0.f
                    ? _geometryParameters.getRadiusCorrection()
                    : radius * _geometryParameters.getRadiusMultiplier());
    }

    /**
     * @brief _getSectionTypes converts Brayns section types into brain::neuron
     * section types
     * @param morphologySectionTypes Brayns section types
     * @return brain::neuron section types
     */
    brain::neuron::SectionTypes _getSectionTypes(
        const size_t morphologySectionTypes) const
    {
        brain::neuron::SectionTypes sectionTypes;
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::soma))
            sectionTypes.push_back(brain::neuron::SectionType::soma);
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::axon))
            sectionTypes.push_back(brain::neuron::SectionType::axon);
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::dendrite))
            sectionTypes.push_back(brain::neuron::SectionType::dendrite);
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::apical_dendrite))
            sectionTypes.push_back(brain::neuron::SectionType::apicalDendrite);
        return sectionTypes;
    }

    /**
     * @brief _getIndexAsTextureCoordinates converts a uint64_t index into 2
     * floats so that it can be stored in the texture coordinates of the the
     * geometry to which it is attached
     * @param index Index to be stored in texture coordinates
     * @return Texture coordinates for the given index
     */
    Vector2f _getIndexAsTextureCoordinates(const uint64_t index) const
    {
        Vector2f textureCoordinates;

        // https://stackoverflow.com/questions/2810280
        float x = ((index & 0xFFFFFFFF00000000LL) >> 32) / INDEX_MAGIC;
        float y = (index & 0xFFFFFFFFLL) / INDEX_MAGIC;

        textureCoordinates.x() = x;
        textureCoordinates.y() = y;
        return textureCoordinates;
    }

    /**
     * @brief _importMorphologyAsPoint places sphere at the specified morphology
     * position
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters do
     * not apply
     * @param compartmentReport Compartment report to map to the morphology
     * @param scene Scene to which the morphology should be loaded into
     * @return True if the loading was successful, false otherwise
     */
    bool _importMorphologyAsPoint(const uint64_t index,
                                  MaterialFunc materialFunc,
                                  const Matrix4f& transformation,
                                  CompartmentReportPtr compartmentReport,
                                  ParallelModelContainer& model)
    {
        uint64_t offset = 0;
        if (compartmentReport)
            offset = compartmentReport->getOffsets()[index][0];

        const auto radius = _geometryParameters.getRadiusMultiplier();
        const auto textureCoordinates = _getIndexAsTextureCoordinates(offset);
        const auto somaPosition = transformation.getTranslation();
        const auto materialId = materialFunc(brain::neuron::SectionType::soma);
        model.addSphere(materialId,
                        {somaPosition, radius, 0.f, textureCoordinates});
        return true;
    }

    /**
     * @brief _createRealisticSoma Creates a realistic soma using the metaballs
     * algorithm.
     * @param uri URI of the morphology for which the soma is created
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters
     * do not apply
     * @param scene Scene to which the morphology should be loaded into
     * @return True if the loading was successful, false otherwise
     */
    bool _createRealisticSoma(const servus::URI& uri, MaterialFunc materialFunc,
                              const Matrix4f& transformation,
                              ParallelModelContainer& model)
    {
        try
        {
            const size_t morphologySectionTypes =
                enumsToBitmask(_geometryParameters.getMorphologySectionTypes());

            brain::neuron::Morphology morphology(uri, transformation);
            const auto sectionTypes = _getSectionTypes(morphologySectionTypes);
            const auto& sections = morphology.getSections(sectionTypes);

            Vector4fs metaballs;
            if (morphologySectionTypes & size_t(MorphologySectionType::soma))
            {
                // Soma
                const auto& soma = morphology.getSoma();
                const auto center = soma.getCentroid();
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                metaballs.push_back(
                    Vector4f(center.x(), center.y(), center.z(), radius));
                model.bounds.merge(center);
            }

            // Dendrites and axon
            for (const auto& section : sections)
            {
                const auto hasParent = section.hasParent();
                if (hasParent)
                {
                    const auto parentSectionType =
                        section.getParent().getType();
                    if (parentSectionType != brain::neuron::SectionType::soma)
                        continue;
                }

                const auto& samples = section.getSamples();
                if (samples.empty())
                    continue;

                const auto samplesFromSoma =
                    _geometryParameters.getMetaballsSamplesFromSoma();
                const auto samplesToProcess =
                    std::min(samplesFromSoma, samples.size());
                for (size_t i = 0; i < samplesToProcess; ++i)
                {
                    const auto& sample = samples[i];
                    const Vector3f position(sample.x(), sample.y(), sample.z());
                    const auto radius = _getCorrectedRadius(sample.w() * 0.5f);
                    if (radius > 0.f)
                        metaballs.push_back(Vector4f(position.x(), position.y(),
                                                     position.z(), radius));

                    model.bounds.merge(position);
                }
            }

            // Generate mesh from metaballs
            const auto gridSize = _geometryParameters.getMetaballsGridSize();
            const auto threshold = _geometryParameters.getMetaballsThreshold();
            MetaballsGenerator metaballsGenerator;
            const auto materialId =
                materialFunc(brain::neuron::SectionType::soma);
            metaballsGenerator.generateMesh(metaballs, gridSize, threshold,
                                            materialId, model.trianglesMeshes);
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << e.what() << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief _importMorphologyFromURI imports a morphology from the specified
     * URI
     * @param uri URI of the morphology
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters
     * do not apply
     * @param compartmentReport Compartment report to map to the morphology
     * @param scene Scene to which the morphology should be loaded into
     * @return True if the loading was successful, false otherwise
     */
    bool _importMorphologyFromURI(const servus::URI& uri, const uint64_t index,
                                  MaterialFunc materialFunc,
                                  const Matrix4f& transformation,
                                  CompartmentReportPtr compartmentReport,
                                  ParallelModelContainer& model) const
    {
        try
        {
            Vector3f translation;

            const size_t morphologySectionTypes =
                enumsToBitmask(_geometryParameters.getMorphologySectionTypes());

            brain::neuron::Morphology morphology(uri, transformation);
            brain::neuron::SectionTypes sectionTypes;

            const MorphologyLayout& layout =
                _geometryParameters.getMorphologyLayout();

            if (layout.nbColumns != 0)
            {
                Boxf morphologyAABB;
                const auto& points = morphology.getPoints();
                for (const auto& point : points)
                    morphologyAABB.merge({point.x(), point.y(), point.z()});

                const Vector3f positionInGrid = {
                    -1.f * layout.horizontalSpacing *
                        static_cast<float>(index % layout.nbColumns),
                    -1.f * layout.verticalSpacing *
                        static_cast<float>(index / layout.nbColumns),
                    0.f};
                translation = positionInGrid - morphologyAABB.getCenter();
            }

            sectionTypes = _getSectionTypes(morphologySectionTypes);

            uint64_t offset = 0;

            if (compartmentReport)
                offset = compartmentReport->getOffsets()[index][0];

            // Soma
            if (!_geometryParameters.useRealisticSomas() &&
                morphologySectionTypes &
                    static_cast<size_t>(MorphologySectionType::soma))
            {
                const auto& soma = morphology.getSoma();
                const size_t materialId =
                    materialFunc(brain::neuron::SectionType::soma);
                const auto somaPosition = soma.getCentroid() + translation;
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                const auto textureCoordinates =
                    _getIndexAsTextureCoordinates(offset);
                model.addSphere(materialId, {somaPosition, radius, 0.f,
                                             textureCoordinates});

                if (_geometryParameters.getCircuitUseSimulationModel())
                {
                    // When using a simulation model, parametric geometries must
                    // occupy as much space as possible in the mesh. This code
                    // inserts a Cone between the soma and the beginning of each
                    // branch.
                    const auto& children = soma.getChildren();
                    for (const auto& child : children)
                    {
                        const auto& samples = child.getSamples();
                        const Vector3f sample{samples[0].x(), samples[0].y(),
                                              samples[0].z()};
                        model.addCone(materialId, {somaPosition, sample, radius,
                                                   _getCorrectedRadius(
                                                       samples[0].w() * 0.5f),
                                                   0.f, textureCoordinates});
                    }
                }
            }

            // Only the first one or two axon sections are reported, so find the
            // last one and use its offset for all the other axon sections
            uint16_t lastAxon = 0;
            if (compartmentReport &&
                (morphologySectionTypes &
                 static_cast<size_t>(MorphologySectionType::axon)))
            {
                const auto& counts =
                    compartmentReport->getCompartmentCounts()[index];
                const auto& axon =
                    morphology.getSections(brain::neuron::SectionType::axon);
                for (const auto& section : axon)
                {
                    if (counts[section.getID()] > 0)
                    {
                        lastAxon = section.getID();
                        continue;
                    }
                    break;
                }
            }

            // Dendrites and axon
            for (const auto& section : morphology.getSections(sectionTypes))
            {
                if (section.getType() == brain::neuron::SectionType::soma)
                    continue;

                const auto materialId = materialFunc(section.getType());
                const auto& samples = section.getSamples();
                if (samples.empty())
                    continue;

                auto previousSample = samples[0];
                size_t step = 1;
                switch (_geometryParameters.getGeometryQuality())
                {
                case GeometryQuality::low:
                    step = samples.size() - 1;
                    break;
                case GeometryQuality::medium:
                    step = samples.size() / 2;
                    step = (step == 0) ? 1 : step;
                    break;
                default:
                    step = 1;
                }

                const float distanceToSoma = section.getDistanceToSoma();
                const floats& distancesToSoma =
                    section.getSampleDistancesToSoma();

                float segmentStep = 0.f;
                if (compartmentReport)
                {
                    const auto& counts =
                        compartmentReport->getCompartmentCounts()[index];
                    // Number of compartments usually differs from number of
                    // samples
                    segmentStep =
                        counts[section.getID()] / float(samples.size());
                }

                bool done = false;
                for (size_t i = step; !done && i < samples.size() + step;
                     i += step)
                {
                    if (i >= samples.size())
                    {
                        i = samples.size() - 1;
                        done = true;
                    }

                    const auto distance = distanceToSoma + distancesToSoma[i];

                    if (compartmentReport)
                    {
                        const auto& offsets =
                            compartmentReport->getOffsets()[index];
                        const auto& counts =
                            compartmentReport->getCompartmentCounts()[index];

                        // update the offset if we have enough compartments aka
                        // a full compartment report. Otherwise we keep the soma
                        // offset which happens for soma reports and use this
                        // for all the sections
                        if (section.getID() < counts.size())
                        {
                            if (counts[section.getID()] > 0)
                                offset = offsets[section.getID()] +
                                         float(i - step) * segmentStep;
                            else
                            {
                                if (section.getType() ==
                                    brain::neuron::SectionType::axon)
                                {
                                    offset = offsets[lastAxon];
                                }
                                else
                                    // This should never happen, but just in
                                    // case use an invalid value to show an
                                    // error color
                                    offset =
                                        std::numeric_limits<uint64_t>::max();
                            }
                        }
                    }

                    const auto sample = samples[i];
                    const auto previousRadius =
                        _getCorrectedRadius(samples[i - step].w() * 0.5f);

                    Vector3f position(sample.x(), sample.y(), sample.z());
                    position += translation;
                    Vector3f target(previousSample.x(), previousSample.y(),
                                    previousSample.z());
                    target += translation;
                    const auto textureCoordinates =
                        _getIndexAsTextureCoordinates(offset);
                    const auto radius =
                        _getCorrectedRadius(samples[i].w() * 0.5f);

                    if (radius > 0.f)
                    {
                        model.addSphere(materialId, {position, radius, distance,
                                                     textureCoordinates});

                        if (position != target && previousRadius > 0.f)
                        {
                            if (radius == previousRadius)
                                model.addCylinder(materialId,
                                                  {position, target, radius,
                                                   distance,
                                                   textureCoordinates});
                            else
                                model.addCone(materialId,
                                              {position, target, radius,
                                               previousRadius, distance,
                                               textureCoordinates});
                        }
                    }
                    previousSample = sample;
                }
            }
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << e.what() << std::endl;
            return false;
        }
        return true;
    }

private:
    const GeometryParameters& _geometryParameters;
};

MorphologyLoader::MorphologyLoader(Scene& scene,
                                   const GeometryParameters& geometryParameters)
    : Loader(scene)
    , _impl(new MorphologyLoader::Impl(geometryParameters))
{
}

MorphologyLoader::~MorphologyLoader()
{
}

std::set<std::string> MorphologyLoader::getSupportedDataTypes()
{
    return {"h5", "swc"};
}

ModelDescriptorPtr MorphologyLoader::importFromBlob(Blob&& /*blob*/,
                                                    const size_t /*index*/,
                                                    const size_t /*materialID*/)
{
    throw std::runtime_error("Load morphology from memory not supported");
}

ModelDescriptorPtr MorphologyLoader::importFromFile(
    const std::string& fileName, const size_t index,
    const size_t defaultMaterialId BRAYNS_UNUSED)
{
    const auto modelName = boost::filesystem::basename({fileName});
    updateProgress("Loading " + modelName + " ...", 0, 100);
    auto model = _scene.createModel();
    importMorphology(servus::URI(fileName), *model, index);
    model->createMissingMaterials();
    updateProgress("Loading " + modelName + " ...", 100, 100);
    return std::make_shared<ModelDescriptor>(std::move(model), fileName);
}

bool MorphologyLoader::importMorphology(const servus::URI& uri, Model& model,
                                        const size_t index,
                                        const Matrix4f& transformation)
{
    return _impl->importMorphology(uri, model, index, transformation);
}

bool MorphologyLoader::_importMorphology(const servus::URI& source,
                                         const uint64_t index,
                                         MaterialFunc materialFunc,
                                         const Matrix4f& transformation,
                                         CompartmentReportPtr compartmentReport,
                                         ParallelModelContainer& model)
{
    return _impl->importMorphology(source, index, materialFunc, transformation,
                                   compartmentReport, model);
}
}
