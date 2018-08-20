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

#include "ParametersManager.h"

#include <brayns/parameters/AbstractParameters.h>

#include <boost/algorithm/string.hpp>

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
    output.insert(std::end(output), std::begin(levenshteinOptions),
                  std::end(levenshteinOptions));
    output.resize(std::min(output.size(), MAX_SUGGESTIONS));

    return output;
}
}

namespace brayns
{
ParametersManager::ParametersManager()
{
    registerParameters(&_animationParameters);
    registerParameters(&_applicationParameters);
    registerParameters(&_geometryParameters);
    registerParameters(&_renderingParameters);
    registerParameters(&_sceneParameters);
    registerParameters(&_streamParameters);
    registerParameters(&_volumeParameters);

    for (auto parameters : _parameterSets)
        _parameters.add(parameters->parameters());
}

void ParametersManager::registerParameters(AbstractParameters* parameters)
{
    _parameterSets.push_back(parameters);
}

void ParametersManager::parse(int argc, const char** argv)
{
    bool help = false;
    for (int i = 0; i < argc && !help; ++i)
        if (std::string(argv[i]) == "--help")
            help = true;

    if (help)
    {
        usage();
        exit(EXIT_SUCCESS);
        return;
    }

    try
    {
        po::variables_map vm;
        po::parsed_options parsedOptions =
            po::command_line_parser(argc, argv)
                .options(_parameters)
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

        for (auto parameters : _parameterSets)
            parameters->parse(vm);
    }
    catch (po::error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ParametersManager::usage()
{
    std::cout << _parameters << std::endl;
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

GeometryParameters& ParametersManager::getGeometryParameters()
{
    return _geometryParameters;
}

SceneParameters& ParametersManager::getSceneParameters()
{
    return _sceneParameters;
}

StreamParameters& ParametersManager::getStreamParameters()
{
    return _streamParameters;
}

VolumeParameters& ParametersManager::getVolumeParameters()
{
    return _volumeParameters;
}

void ParametersManager::set(const std::string& key, const std::string& value)
{
    const std::string p = "--" + key;
    strings strs;
    boost::split(strs, value, boost::is_any_of(" "));

    const size_t argc = 2 + strs.size();
    auto argv = std::make_unique<const char* []>(argc);
    argv[0] = "";
    argv[1] = p.c_str();
    for (size_t i = 0; i < strs.size(); ++i)
        argv[2 + i] = strs[i].c_str();

    parse(argc, argv.get());
}

void ParametersManager::_processUnrecognizedOptions(
    const std::vector<std::string>& unrecognizedOptions) const
{
    if (!unrecognizedOptions.empty())
    {
        const auto& unrecognized = unrecognizedOptions.front();

        std::vector<std::string> availableOptions;
        for (auto option : _parameters.options())
            availableOptions.push_back(option->format_name());

        const auto suggestions =
            findSimilarOptions(unrecognized, availableOptions);

        std::string errorMessage = "Unrecognized option '" + unrecognized +
                                   "'.\n\nMost similar options are:";

        for (const auto& suggestion : suggestions)
            errorMessage += "\n\t" + suggestion;

        throw po::error(errorMessage);
    }
}
}
