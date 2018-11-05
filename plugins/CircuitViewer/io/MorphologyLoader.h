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
#include <brayns/parameters/GeometryParameters.h>

#include <brain/neuron/types.h>
#include <brion/types.h>

#include <vector>

namespace servus
{
class URI;
}

namespace brayns
{
class CircuitLoader;
struct ParallelModelContainer;
using GIDOffsets = std::vector<uint64_t>;

/** Loads morphologies from SWC and H5, and Circuit Config files */
class MorphologyLoader : public Loader
{
public:
    MorphologyLoader(Scene& scene,
                     const GeometryParameters& geometryParameters);
    ~MorphologyLoader();

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    ModelDescriptorPtr importFromBlob(Blob&& blob,
                                      const LoaderProgress& callback,
                                      const size_t index,
                                      const size_t materialID) const final;

    ModelDescriptorPtr importFromFile(const std::string& filename,
                                      const LoaderProgress& callback,
                                      const size_t index,
                                      const size_t materialID) const final;

    /**
     * @brief Imports morphology from a given SWC or H5 file
     * @param source URI of the morphology
     * @param index Specifies an index for the morphology. This is mainly used
     * to give a specific color to every morphology
     * @param transformation Transformation to apply to the morphology
     * @return Position of the soma
     */
    Vector3f importMorphology(const servus::URI& source, Model& model,
                              const size_t index,
                              const Matrix4f& transformation) const;

private:
    using CompartmentReportPtr = std::shared_ptr<brion::CompartmentReport>;
    using MaterialFunc = std::function<size_t(brain::neuron::SectionType)>;
    Vector3f _importMorphology(const servus::URI& source, const uint64_t index,
                               MaterialFunc materialFunc,
                               const Matrix4f& transformation,
                               CompartmentReportPtr compartmentReport,
                               ParallelModelContainer& model) const;
    friend class CircuitLoader;
    class Impl;
    std::unique_ptr<const Impl> _impl;
};
}

#endif // MORPHOLOGY_LOADER_H
