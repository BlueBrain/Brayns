/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include <plugin/io/sonataloader/populations/NodePopulationLoader.h>

namespace sonataloader
{
/**
 * @brief The CommonNodeLoader class is the base class for morphology based
 * SONATA circuti loaders, such as the biophysical or astrocyte node
 * population loaders
 */
class CommonNodeLoader : public NodePopulationLoader
{
public:
    template <typename T, typename = std::enable_if_t<
                              std::is_constructible<std::string, T>::value>>
    CommonNodeLoader(T&& name)
        : NodePopulationLoader(std::forward<T>(name))
    {
    }

    /**
     * @brief loads a node population as morphologiy geometry
     * @param loadSettings Settings that configures the load process
     * @param nodeSelection Selection of nodes to laod from within the
     * population
     * @param morphologyNames vector with the morphology name for each selected
     * node
     * @param positions vector with the translation for each selected node
     * @param rotations vector with the rotation for each selected node
     * @param cb progress report callback to inform the client of the load
     * process
     * @return a vector of morphology instances holding the geometries
     */
    std::vector<MorphologyInstance::Ptr> loadNodes(
        const SonataConfig::Data& networkData,
        const SonataNodePopulationParameters& loadSettings,
        const bbp::sonata::Selection& nodeSelection,
        const std::vector<std::string>& morphologyNames,
        const std::vector<brayns::Vector3f>& positions,
        const std::vector<brayns::Quaternion>& rotations) const;
};
} // namespace sonataloader
