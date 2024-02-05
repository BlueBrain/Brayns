/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "ParametersManager.h"

#include <iostream>

#include <brayns/Version.h>
#include <brayns/common/Log.h>
#include <brayns/parameters/AbstractParameters.h>

namespace
{
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C++
unsigned int levenshtein_distance(const std::string& s1, const std::string& s2)
{
    const std::size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

    for (unsigned int i = 0; i < prevCol.size(); i++)
        prevCol[i] = i;
    for (unsigned int i = 0; i < len1; i++)
    {
        col[0] = i + 1;
        for (unsigned int j = 0; j < len2; j++)
            col[j + 1] = std::min({prevCol[1 + j] + 1, col[j] + 1,
                                   prevCol[j] + (s1[i] == s2[j] ? 0 : 1)});
        col.swap(prevCol);
    }
    return prevCol[len2];
}

std::vector<std::string> findSimilarOptions(
    const std::string& name, const std::vector<std::string>& options)
{
    constexpr size_t MAX_SUGGESTIONS = 7;

    std::vector<std::string> subStringOptions;
    std::vector<std::string> levenshteinOptions;

    // Collect substring options
    {
        // Strip dashes
        auto nameStrip = name;
        nameStrip.erase(std::remove(nameStrip.begin(), nameStrip.end(), '-'),
                        nameStrip.end());

        // Suggest options containing the substring
        for (const auto& optionName : options)
        {
            if (subStringOptions.size() >= MAX_SUGGESTIONS)
                break;

            if (optionName.find(nameStrip) != std::string::npos)
                subStringOptions.push_back(optionName);
        }
    }

    // Collect best levenshtein distance options
    {
        size_t bestDist = UINT_MAX;

        for (const auto& optionName : options)
        {
            if (levenshteinOptions.size() >= MAX_SUGGESTIONS)
                break;

            const auto dist = levenshtein_distance(name, optionName);
            if (dist < bestDist)
            {
                levenshteinOptions.clear();
                bestDist = dist;
            }

            if (dist == bestDist &&
                std::find(subStringOptions.begin(), subStringOptions.end(),
                          optionName) == subStringOptions.end())
                levenshteinOptions.push_back(optionName);
        }
    }

    // Merge suggestions giving precedence to substrings
    auto output = subStringOptions;
    for (const auto& option : levenshteinOptions)
        output.push_back(option);
    output.resize(std::min(output.size(), MAX_SUGGESTIONS));

    return output;
}

void _printVersion()
{
    brayns::Log::info("Brayns {}.{}.{} ({})", brayns::Version::getMajor(),
                      brayns::Version::getMinor(), brayns::Version::getPatch(),
                      brayns::Version::getCommitHash());
}
} // namespace

namespace brayns
{
ParametersManager::ParametersManager(const int argc, const char** argv)
{
    registerParameters(&_animationParameters);
    registerParameters(&_applicationParameters);
    registerParameters(&_renderingParameters);
    registerParameters(&_volumeParameters);
    registerParameters(&_networkParameters);

    for (auto parameters : _parameterSets)
        _allOptions.add(parameters->parameters());

    _parse(argc, argv);
}

void ParametersManager::registerParameters(AbstractParameters* parameters)
{
    _parameterSets.push_back(parameters);
}

void ParametersManager::_parse(int argc, const char** argv)
{
    try
    {
        po::options_description generalOptions("General options");
        generalOptions.add_options()("help", "Print this help")(
            "version", "Print the Brayns version")("verbose",
                                                   "Print parsed parameters");

        _allOptions.add(generalOptions);

        po::variables_map vm;
        po::parsed_options parsedOptions =
            po::command_line_parser(argc, argv)
                .options(_allOptions)
                .allow_unregistered()
                .positional(_applicationParameters.posArgs())
                .style(po::command_line_style::unix_style &
                       ~po::command_line_style::allow_short &
                       ~po::command_line_style::allow_guessing)
                .run();

        const auto unrecognizedOptions =
            po::collect_unrecognized(parsedOptions.options,
                                     po::exclude_positional);

        _processUnrecognizedOptions(unrecognizedOptions);

        po::store(parsedOptions, vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            usage();
            exit(EXIT_SUCCESS);
        }

        _printVersion();
        if (vm.count("version"))
            exit(EXIT_SUCCESS);

        for (auto parameters : _parameterSets)
            parameters->parse(vm);

        if (vm.count("verbose"))
            print();
    }
    catch (const po::error& e)
    {
        Log::error(e.what());
        exit(EXIT_FAILURE);
    }
}

void ParametersManager::usage()
{
    std::cout << _allOptions << std::endl;
}

void ParametersManager::print()
{
    for (AbstractParameters* parameters : _parameterSets)
        parameters->print();
}

void ParametersManager::resetModified()
{
    for (AbstractParameters* parameters : _parameterSets)
        parameters->resetModified();
}

bool ParametersManager::isAnyModified() const
{
    for (AbstractParameters* parameters : _parameterSets)
    {
        if (parameters->isModified())
            return true;
    }
    return false;
}

AnimationParameters& ParametersManager::getAnimationParameters()
{
    return _animationParameters;
}

const AnimationParameters& ParametersManager::getAnimationParameters() const
{
    return _animationParameters;
}

ApplicationParameters& ParametersManager::getApplicationParameters()
{
    return _applicationParameters;
}

const ApplicationParameters& ParametersManager::getApplicationParameters() const
{
    return _applicationParameters;
}

RenderingParameters& ParametersManager::getRenderingParameters()
{
    return _renderingParameters;
}

const RenderingParameters& ParametersManager::getRenderingParameters() const
{
    return _renderingParameters;
}

VolumeParameters& ParametersManager::getVolumeParameters()
{
    return _volumeParameters;
}

const VolumeParameters& ParametersManager::getVolumeParameters() const
{
    return _volumeParameters;
}

NetworkParameters& ParametersManager::getNetworkParameters()
{
    return _networkParameters;
}

const NetworkParameters& ParametersManager::getNetworkParameters() const
{
    return _networkParameters;
}

void ParametersManager::_processUnrecognizedOptions(
    const std::vector<std::string>& unrecognizedOptions) const
{
    if (unrecognizedOptions.empty())
        return;

    const auto& unrecognized = unrecognizedOptions.front();

    std::vector<std::string> availableOptions;
    for (auto option : _allOptions.options())
        availableOptions.push_back(option->format_name());

    const auto suggestions = findSimilarOptions(unrecognized, availableOptions);

    std::string errorMessage = "Unrecognized option '" + unrecognized +
                               "'.\n\nMost similar options are:";

    for (const auto& suggestion : suggestions)
        errorMessage += "\n\t" + suggestion;

    throw po::error(errorMessage);
}
} // namespace brayns
