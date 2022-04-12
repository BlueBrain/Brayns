/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "DTIConfiguration.h"

#include <boost/property_tree/ini_parser.hpp>

namespace dtiloader
{
DTIConfiguration DTIConfigurationReader::read(const std::string &path)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(path, pt);

    DTIConfiguration configuration;

    configuration.streamlinesPath = pt.get<std::string>("streamlines");
    configuration.gidsToStreamlinesPath = pt.get<std::string>("gids_to_streamline_row");

    const auto circuitFile = pt.get_optional<std::string>("circuit");
    if (circuitFile.has_value())
    {
        configuration.circuitPath = *circuitFile;
    }

    return configuration;
}
}
