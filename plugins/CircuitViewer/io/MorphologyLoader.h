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

#ifndef MORPHOLOGY_LOADER_H
#define MORPHOLOGY_LOADER_H

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>

#include <brain/neuron/types.h>
#include <brain/types.h>
#include <brion/types.h>

#include <vector>

namespace brayns
{
class CircuitLoader;
struct ModelData;

enum class NeuronColorScheme
{
    none = 0,
    by_id = 1,
    by_segment_type = 2,
    by_layer = 3,
    by_mtype = 4,
    by_etype = 5,
    by_target = 6,
};

enum class NeuronDisplayMode
{
    soma = 0,
    no_axon = 1,
    full = 2,
};

struct MorphologyLoaderParams
{
    MorphologyLoaderParams() = default;
    MorphologyLoaderParams(const PropertyMap& properties);

    NeuronColorScheme colorScheme = NeuronColorScheme::none;
    double radiusMultiplier = 0.0;
    double radiusCorrection = 0.0;
    NeuronDisplayMode mode;
    bool dampenBranchThicknessChangerate = false;
    bool useSDFGeometries = false;
    GeometryQuality geometryQuality = GeometryQuality::high;
};

/** Loads morphologies from SWC and H5, and Circuit Config files */
class MorphologyLoader : public Loader
{
public:
    MorphologyLoader(Scene& scene, PropertyMap defaultParams);
    ~MorphologyLoader();

    strings getSupportedExtensions() const final;
    std::string getName() const final;
    static PropertyMap getCLIProperties();
    PropertyMap getProperties() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    ModelDescriptorPtr importFromBlob(Blob&& blob,
                                      const LoaderProgress& callback,
                                      const PropertyMap& properties,
                                      const size_t index,
                                      const size_t materialID) const final;

    ModelDescriptorPtr importFromFile(const std::string& filename,
                                      const LoaderProgress& callback,
                                      const PropertyMap& properties,
                                      const size_t index,
                                      const size_t materialID) const final;

    using MaterialFunc = std::function<size_t(brain::neuron::SectionType)>;

    static ModelData processMorphology(
        const brain::neuron::Morphology& morphology, const uint64_t index,
        MaterialFunc materialFunc,
        const brain::CompartmentReportMapping* reportMapping,
        const MorphologyLoaderParams& params);

private:
    friend class CircuitLoader;
    class Impl;

    PropertyMap _defaults; // command line defaults
};

void createMissingMaterials(Model& model);
}

#endif // MORPHOLOGY_LOADER_H
