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

#include <plugin/io/sonataloader/populations/EdgePopulationLoader.h>

namespace sonataloader
{
/**
 * @brief The CommonEdgePopulationLoader class implements the edge load
 * functionality to read a wide variety of edge population types that share the
 * same mandatory datasets, allowing to have a common functionality.
 * Nonetheless, and even though all such functionality is implemented on this
 * class, it exists a class for one of each of these types of edge populations,
 * that simply extend this one. This is done so its easier to understand the
 * code workflow.
 */
class CommonEdgeLoader : public EdgePopulationLoader
{
public:
    template <typename T, typename = std::enable_if_t<
                              std::is_constructible<std::string, T>::value>>
    CommonEdgeLoader(T&& name)
        : EdgePopulationLoader(std::forward<T>(name))
    {
    }

    std::vector<SynapseGroup::Ptr> load(
        const SonataConfig::Data& networkConfig,
        const SonataEdgePopulationParameters& lc,
        const bbp::sonata::Selection& nodeSelection) const final;
};
} // namespace sonataloader
