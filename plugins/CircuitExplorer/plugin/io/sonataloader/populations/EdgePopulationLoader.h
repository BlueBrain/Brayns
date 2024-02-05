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

#include <plugin/io/SonataLoaderParameters.h>
#include <plugin/io/sonataloader/data/SonataConfig.h>
#include <plugin/io/synapse/SynapseGroup.h>

namespace sonataloader
{
/**
 * @brief The EdgePopulationLoader is the base class for implementations that
 * load SONATA edge populations which must be transformed into SynapseGroups (in
 * other words, transforms edge population data into scene geometry)
 */
class EdgePopulationLoader
{
public:
    using Ptr = std::unique_ptr<EdgePopulationLoader>;

    template <typename T, typename = std::enable_if_t<
                              std::is_constructible<std::string, T>::value>>
    EdgePopulationLoader(T&& name)
        : _typeName(std::forward<T>(name))
    {
    }

    virtual ~EdgePopulationLoader() = default;

    /**
     * @brief returns a std::string representing the type of edge population
     * of this loader
     */
    const std::string& getType() const noexcept { return _typeName; }

    /**
     * @brief load the edge population data. The given parameters may be used to
     * configure the load process. The SubProgressReport allows to notify
     * progress to listening clients of the Brayns API
     */
    virtual std::vector<SynapseGroup::Ptr> load(
        const SonataConfig::Data& networkConfig,
        const SonataEdgePopulationParameters& lc,
        const bbp::sonata::Selection& nodeSelection) const = 0;

private:
    const std::string _typeName;
};
} // namespace sonataloader
