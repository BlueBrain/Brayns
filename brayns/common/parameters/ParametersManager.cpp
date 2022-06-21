/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/parameters/ParametersManager.h>

#include <sstream>

#include <brayns/Version.h>

#include "argv/ArgvParser.h"

namespace
{
class ArgvPropertyBuilder
{
public:
    static std::vector<brayns::ArgvProperty> build(brayns::ParametersManager &manager)
    {
        std::vector<brayns::ArgvProperty> properties;
        brayns::ArgvBuilder builder(properties);
        manager.forEach([&](auto &parameters) { parameters.build(builder); });
        return properties;
    }
};

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
        return first == "-v" || first == "-?" || first == "--version";
    }
};

class VersionFormatter
{
public:
    static std::string format()
    {
        std::ostringstream stream;
        stream << "Brayns version ";
        stream << brayns::Version::getMajor() << ".";
        stream << brayns::Version::getMinor() << ".";
        stream << brayns::Version::getPatch();
        stream << " (" << brayns::Version::getCommitHash() << ")";
        stream << " Copyright (c) 2015-2022 EPFL/Blue Brain Project";
        return stream.str();
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
        return first == "-h" || first == "--help";
    }
};

class HelpFormatter
{
public:
    static std::string format(const std::vector<brayns::ArgvProperty> &properties)
    {
        std::ostringstream stream;
        stream << VersionFormatter::format() << '\n';
        for (const auto &property : properties)
        {
            stream << '\n';
            _format(stream, property);
        }
        stream << "\n\n";
        stream << "Use '-v' or '--version' to get version info.\n";
        stream << "Use '-h' or '-?' or '--help' to get the list of available commands.\n";
        return stream.str();
    }

private:
    static void _format(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        stream << property.name << " ";
        stream << brayns::EnumInfo::getName(property.type);
        stream << " (default = " << property.stringify();
        stream << "): " << property.description;
        _formatOptions(stream, property);
    }

    static void _formatOptions(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        if (property.multitoken)
        {
            stream << "\nMultitoken: this property accepts multiple tokens grouped using quotes.";
        }
        if (property.composable)
        {
            stream << "\nComposable: this property can be specified multiple times to make a list.";
        }
        _formatItemCount(stream, property);
        _formatBounds(stream, property);
    }

    static void _formatItemCount(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        if (property.minItems)
        {
            stream << "\nMinimum items: this property requires at least " << *property.minItems << " items.";
        }
        if (property.maxItems)
        {
            stream << "\nMaximum items: this property accepts at most " << *property.maxItems << " items.";
        }
    }

    static void _formatBounds(std::ostream &stream, const brayns::ArgvProperty &property)
    {
        if (property.minimum)
        {
            stream << "\nMinimum: the value(s) of this property must be >= " << *property.minimum << ".";
        }
        if (property.maximum)
        {
            stream << "\nMaximum: the value(s) of this property must be <= " << *property.maximum << ".";
        }
    }
};
} // namespace

namespace brayns
{
ParametersManager::ParametersManager(int argc, const char **argv)
{
    _version = VersionFinder::find(argc, argv);
    _help = HelpFinder::find(argc, argv);
    _properties = ArgvPropertyBuilder::build(*this);
    if (_version || _help)
    {
        return;
    }
    ArgvParser parser(_properties);
    parser.parse(argc, argv);
}

bool ParametersManager::hasVersion()
{
    return _version;
}

std::string ParametersManager::getVersion()
{
    return VersionFormatter::format();
}

bool ParametersManager::hasHelp()
{
    return _help;
}

std::string ParametersManager::getHelp()
{
    return HelpFormatter::format(_properties);
}

SimulationParameters &ParametersManager::getSimulationParameters()
{
    return _simulationParameters;
}

const SimulationParameters &ParametersManager::getSimulationParameters() const
{
    return _simulationParameters;
}

ApplicationParameters &ParametersManager::getApplicationParameters()
{
    return _applicationParameters;
}

const ApplicationParameters &ParametersManager::getApplicationParameters() const
{
    return _applicationParameters;
}

NetworkParameters &ParametersManager::getNetworkParameters()
{
    return _networkParameters;
}

const NetworkParameters &ParametersManager::getNetworkParameters() const
{
    return _networkParameters;
}

void ParametersManager::resetModified()
{
    forEach([](auto &parameters) { parameters.resetModified(); });
}
} // namespace brayns
