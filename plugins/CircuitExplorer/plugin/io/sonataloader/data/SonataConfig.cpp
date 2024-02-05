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

#include "SonataConfig.h"

#include <brayns/utils/Filesystem.h>

namespace sonataloader
{
namespace
{
constexpr char H5_FORMAT[] = "h5v1";
constexpr char ASCII_FORMAT[] = "neurolucida-asc";
} // namespace

SonataConfig::Data SonataConfig::readNetwork(const std::string& configPath)
{
    return SonataConfig::Data{configPath,
                              bbp::sonata::CircuitConfig::fromFile(configPath)};
}

SonataConfig::MorphologyPath::MorphologyPath(const std::string& path,
                                             const char* extension)
    : _path(path)
    , _extension(extension)
{
}

std::string SonataConfig::MorphologyPath::buildPath(
    const std::string& morphologyName) const noexcept
{
    return _path + "/" + morphologyName + "." + _extension;
}

SonataConfig::MorphologyPath SonataConfig::resolveMorphologyPath(
    const bbp::sonata::PopulationProperties& properties)
{
    if (fs::exists(properties.morphologiesDir))
        return SonataConfig::MorphologyPath(properties.morphologiesDir, "swc");

    const auto h5It = properties.alternateMorphologyFormats.find(H5_FORMAT);
    if (h5It != properties.alternateMorphologyFormats.end() &&
        fs::exists(h5It->second))
        return SonataConfig::MorphologyPath(h5It->second, "h5");

    const auto ascIt = properties.alternateMorphologyFormats.find(ASCII_FORMAT);
    if (ascIt != properties.alternateMorphologyFormats.end() &&
        fs::exists(ascIt->second))
        return SonataConfig::MorphologyPath(ascIt->second, "asc");

    throw std::runtime_error(
        "SonataConfig: Could not determine morphology path");
}
} // namespace sonataloader
