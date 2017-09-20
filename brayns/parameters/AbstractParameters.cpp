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

#include "AbstractParameters.h"
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <brayns/common/types.h>
#include <ostream>

namespace brayns
{
bool AbstractParameters::parse(int argc, const char** argv)
{
    for (int i = 1; i < argc; ++i)
        _arguments.push_back(argv[i]);

    try
    {
        po::variables_map vm;
        po::parsed_options parsedOptions =
            po::command_line_parser(argc, argv)
                .options(_parameters)
                .style(po::command_line_style::unix_style ^
                       po::command_line_style::allow_short)
                .allow_unregistered()
                .run();
        po::store(parsedOptions, vm);
        po::notify(vm);
        _parse(vm);
    }
    catch (po::error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return false;
    }

    return true;
}

void AbstractParameters::usage()
{
    std::cout << _name << " parameters:" << std::endl
              << std::endl
              << _parameters << std::endl;
}

void AbstractParameters::print()
{
    BRAYNS_INFO << "-= " << _name << " parameters =-" << std::endl;
}

const strings& AbstractParameters::arguments() const
{
    return _arguments;
}

void AbstractParameters::set(const std::string& key, const std::string& value)
{
    const std::string p = "--" + key;
    std::vector<std::string> strs;
    boost::split(strs, value, boost::is_any_of(" "));

    const size_t argc = 2 + strs.size();
    const char** argv = new const char*[argc];
    argv[0] = "";
    argv[1] = p.c_str();
    for (size_t i = 0; i < strs.size(); ++i)
        argv[2 + i] = strs[i].c_str();

    po::variables_map vm;
    po::parsed_options parsedOptions =
        po::command_line_parser(argc, argv)
            .options(_parameters)
            .style(po::command_line_style::unix_style ^
                   po::command_line_style::allow_short)
            .allow_unregistered()
            .run();
    po::store(parsedOptions, vm);
    po::notify(vm);
    delete[] argv;

    _parse(vm);

    _modified = true;
}
}
