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
#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/sonataloader/data/SonataConfig.h>

namespace sonataloader
{
/**
 * @brief The NodePopulationLoader is the base class for implementations that
 * load SONATA node populations which must be transformed into morphology
 * instances (in other words, transforms node population data into scene
 * geometry)
 */
class NodePopulationLoader
{
public:
    using Ptr = std::unique_ptr<NodePopulationLoader>;

    template <typename T, typename = std::enable_if_t<
                              std::is_constructible<std::string, T>::value>>
    NodePopulationLoader(T&& typeName)
        : _typeName(std::forward<T>(typeName))
    {
    }

    virtual ~NodePopulationLoader() = default;

    /**
     * @brief returns a std::string that represents the population type of this
     * loader
     */
    const std::string& getType() const noexcept { return _typeName; }

    /**
     * @brief load the node population data. The given parameters may be used to
     * configure the load process. The SubProgressReport allows to notify
     * progress to listening clients of the Brayns API
     */
    virtual std::vector<MorphologyInstance::Ptr> load(
        const SonataConfig::Data& networkData,
        const SonataNodePopulationParameters& loadSettings,
        const bbp::sonata::Selection& nodeSelection) const = 0;

private:
    const std::string _typeName;
};
} // namespace sonataloader
