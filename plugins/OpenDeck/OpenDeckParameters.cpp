/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "OpenDeckParameters.h"

#include <boost/program_options.hpp>

#include <brayns/common/log.h>

namespace brayns
{
OpenDeckParameters::OpenDeckParameters()
    : _props("OpenDeck plugin parameters")
{
    _props.add({PARAM_RESOLUTION_SCALING,
                1.0,
                {"OpenDeck native resolution scale",
                 "OpenDeck native resolution scale"}});
    _props.add({PARAM_CAMERA_SCALING,
                1.0,
                {"OpenDeck camera scaling", "OpenDeck camera scaling"}});
}

bool OpenDeckParameters::parse(int argc, const char** argv)
{
    namespace po = boost::program_options;

    try
    {
        po::variables_map properties;

        po::store(po::command_line_parser(argc, argv).run(), properties);
        po::notify(properties);

        auto i = properties.find(PARAM_RESOLUTION_SCALING);
        if (i != properties.end())
        {
            setResolutionScaling(i->second.as<double>());
        }

        i = properties.find(PARAM_CAMERA_SCALING);
        if (i != properties.end())
        {
            setCameraScaling(i->second.as<double>());
        }
    }
    catch (const std::exception& e)
    {
        BRAYNS_ERROR << "Failed to parse commandline for "
                     << std::quoted(_props.getName()) << ": " << e.what()
                     << '\n';
        return false;
    }

    return true;
}
} // namespace brayns
