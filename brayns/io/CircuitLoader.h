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
    CircuitLoader(Scene& scene,
                  const ApplicationParameters& applicationParameters,
                  const GeometryParameters& geometryParameters);
    ~CircuitLoader();

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
    ModelDescriptorPtr importCircuit(const servus::URI& circuitConfig,
                                     const LoaderProgress& callback,
                                     const strings& targets,
                                     const std::string& report);

private:
    class Impl;
    std::unique_ptr<const Impl> _impl;
};
}
