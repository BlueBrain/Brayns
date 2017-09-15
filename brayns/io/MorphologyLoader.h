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

#include <brayns/common/geometry/Primitive.h>
#include <brayns/common/types.h>
#include <brayns/io/MeshLoader.h>
#include <brayns/parameters/GeometryParameters.h>

#include <vector>

namespace servus
{
class URI;
}

namespace brayns
{
/** Loads morphologies from SWC and H5, and Circuit Config files
 */
class MorphologyLoader
{
public:
    /**
     * @brief MorphologyLoader
     * @param geometryParameters
     * @param scene
     */
    MorphologyLoader(const GeometryParameters& geometryParameters,
                     Scene& scene);
    ~MorphologyLoader();

    /**
     * @brief Imports morphology from a given SWC or H5 file
     * @param source URI of the morphology
     * @param index Specifies an index for the morphology. This is mainly used
     * to give a specific color to every morphology
     * @param transformation Transformation to apply to the morphology
     * @return True if the morphology is successfully loaded, false otherwise
     */
    bool importMorphology(const servus::URI& source, const uint64_t index,
                          const size_t material,
                          const Matrix4f& transformation = Matrix4f());

    /**
     * @brief Imports morphology from a circuit for the given target name
     * @param circuitConfig URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     * circuit configuration file is used. If such an entry does not exist, all
     * neurons are loaded.
     * @param report Compartment report to be loaded
     * @param scene Scene into which the circuit is imported
     * @return True if the circuit is successfully loaded, false if the circuit
     * contains no cells.
     */
    bool importCircuit(const servus::URI& circuitConfig,
                       const std::string& target, const std::string& report,
                       Scene& scene, MeshLoader& meshLoader);

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
}

#endif // MORPHOLOGY_LOADER_H
