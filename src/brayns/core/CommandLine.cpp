/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "CommandLine.h"

#include <sstream>
#include <string_view>

#include <brayns/core/Version.h>

#include <brayns/core/parameters/ParametersManager.h>

#include <brayns/core/utils/string/StringJoiner.h>

namespace
{
class VersionFinder
{
public:
    static bool find(int argc, const char **argv)
    {
        if (argc <= 1)
        {
            return false;
        }
        auto first = std::string_view(argv[1]);
        return first == "-v" || first == "--version";
    }
};

class HelpFinder
{
public:
    static bool find(int argc, const char **argv)
    {
        if (argc <= 1)
        {
            return false;
        }
        auto first = std::string_view(argv[1]);
        return first == "-h" || first == "-?" || first == "--help";
    }
};

class HelpFormatter
{
public:
    static std::string format(const std::vector<brayns::ArgvProperty> &properties)
    {
        std::ostringstream stream;
        stream << brayns::Version::getCopyright() << '.';
        stream << "\n\nBrayns can be configured using command line options with keyword arguments (--name value).";
        stream << "\n\nExample: braynsService --uri 0.0.0.0:5000 --window-size '1920 1080'";
        stream << "\n\nThe supported options are the following:";
        for (const auto &property : properties)
        {
            stream << "\n\n";
            _format(stream, property);
        }
        stream << "\n\n";
        stream << "Use '-v' or '--version' to get version info.\n";
        stream << "Use '-h' or '-?' or '--help' to get the list of available commands.";
        return stream.str();
    }

private:
    static void _format(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        stream << property.name;
        stream << " [" << brayns::EnumInfo::getName(property.type) << "]";
        stream << ": " << property.description;
        stream << " (default = '" << property.stringify() << "').";
        _formatOptions(stream, property);
    }

    static void _formatOptions(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        _formatEnums(stream, property);
        if (property.multitoken)
        {
            stream << "\n    Multitoken: this option accepts multiple tokens grouped using quotes.";
        }
        if (property.composable)
        {
            stream << "\n    Composable: this option can be specified multiple times to make a list.";
        }
        if (property.multitoken || property.composable)
        {
            _formatItemCount(stream, property);
        }
        _formatBounds(stream, property);
    }

    static void _formatEnums(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        auto &enums = property.enums;
        if (enums.empty())
        {
            return;
        }
        stream << "\n    Enumeration: this option must be one of the following values: [";
        stream << brayns::StringJoiner::join(enums, ", ") << "].";
    }

    static void _formatItemCount(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        if (property.minItems)
        {
            stream << "\n    Minimum items: this option requires at least " << *property.minItems << " items.";
        }
        if (property.maxItems)
        {
            stream << "\n    Maximum items: this option accepts at most " << *property.maxItems << " items.";
        }
    }

    static void _formatBounds(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        if (property.minimum)
        {
            stream << "\n    Minimum: the value(s) of this option must be >= " << *property.minimum << ".";
        }
        if (property.maximum)
        {
            stream << "\n    Maximum: the value(s) of this option must be <= " << *property.maximum << ".";
        }
    }
};
} // namespace

namespace brayns
{
std::string CommandLine::getHelp()
{
    auto parameters = brayns::ParametersManager(0, nullptr);
    auto properties = parameters.getArgvProperties();
    return HelpFormatter::format(properties);
}

std::string CommandLine::getVersion()
{
    return Version::getCopyright();
}

CommandLine::CommandLine(int argc, const char **argv):
    _argc(argc),
    _argv(argv)
{
}

bool CommandLine::hasVersion()
{
    return VersionFinder::find(_argc, _argv);
}

bool CommandLine::hasHelp()
{
    return HelpFinder::find(_argc, _argv);
}
} // namespace brayns
