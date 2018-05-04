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

#pragma once

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>

#include <vector>

namespace servus
{
class URI;
}

namespace brayns
{
/**
 * Load circuit from BlueConfig or CircuitConfig file, including simulation.
 */
class CircuitLoader : public Loader
{
public:
    CircuitLoader(const ApplicationParameters& applicationParameters,
                  const GeometryParameters& geometryParameters);
    ~CircuitLoader();

    static std::set<std::string> getSupportedDataTypes();

    void importFromBlob(Blob&& blob, Scene& scene, const size_t index,
                        const Matrix4f& transformation,
                        const size_t materialID) final;

    void importFromFile(const std::string& filename, Scene& scene,
                        const size_t index, const Matrix4f& transformation,
                        const size_t materialID) final;

    /**
     * @brief Imports morphology from a circuit for the given target name
     * @param circuitConfig URI of the Circuit Config file
     * @param targets Targets to be loaded. If empty, the target specified in
     * the circuit configuration file is used. If such an entry does not exist,
     * all neurons are loaded.
     * @param report Compartment report to be loaded
     * @param scene Scene into which the circuit is imported
     * @return True if the circuit is successfully loaded, false if the circuit
     * contains no cells.
     */
    bool importCircuit(const servus::URI& circuitConfig, const strings& targets,
                       const std::string& report, Scene& scene);

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
}
