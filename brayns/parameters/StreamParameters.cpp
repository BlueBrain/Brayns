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

#include "StreamParameters.h"

namespace
{
const std::string PARAM_STREAM_COMPRESSION = "stream-disable-compression";
const std::string PARAM_STREAM_HOST = "stream-host";
const std::string PARAM_STREAM_ID = "stream-id";
const std::string PARAM_STREAM_PORT = "stream-port";
const std::string PARAM_STREAM_QUALITY = "stream-quality";
const std::string PARAM_STREAM_RESIZING = "stream-disable-resizing";
}

namespace brayns
{
StreamParameters::StreamParameters()
    : AbstractParameters("Streaming")
{
    _parameters.add_options()(PARAM_STREAM_COMPRESSION.c_str(),
                              po::bool_switch()->default_value(false),
                              "Disable JPEG compression")(
        PARAM_STREAM_HOST.c_str(), po::value<std::string>(),
        "Hostname of Deflect server")(
        PARAM_STREAM_ID.c_str(), po::value<std::string>(),
        "Name of stream")(PARAM_STREAM_PORT.c_str(), po::value<unsigned>(),
                          "Port of Deflect server")(
        PARAM_STREAM_QUALITY.c_str(), po::value<unsigned>(),
        "JPEG quality of stream")(PARAM_STREAM_RESIZING.c_str(),
                                  po::bool_switch()->default_value(false),
                                  "Disable stream resizing");
}

void StreamParameters::parse(const po::variables_map& vm)
{
    _compression = !vm[PARAM_STREAM_COMPRESSION].as<bool>();
    if (vm.count(PARAM_STREAM_HOST))
        _host = vm[PARAM_STREAM_HOST].as<std::string>();
    if (vm.count(PARAM_STREAM_ID))
        _id = vm[PARAM_STREAM_ID].as<std::string>();
    if (vm.count(PARAM_STREAM_PORT))
        _port = vm[PARAM_STREAM_PORT].as<unsigned>();
    if (vm.count(PARAM_STREAM_QUALITY))
        _quality = vm[PARAM_STREAM_QUALITY].as<unsigned>();
    _resizing = !vm[PARAM_STREAM_RESIZING].as<bool>();
    markModified();
}

void StreamParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Stream compression                : "
                << asString(_compression) << std::endl;
    BRAYNS_INFO << "Stream host                       : " << _host << std::endl;
    BRAYNS_INFO << "Stream ID                         : " << _id << std::endl;
    BRAYNS_INFO << "Stream port                       : " << _port << std::endl;
    BRAYNS_INFO << "Stream quality                    : " << _quality
                << std::endl;
    BRAYNS_INFO << "Stream resizing                   : " << asString(_resizing)
                << std::endl;
}
}
