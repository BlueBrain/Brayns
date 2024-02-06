/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "ParameterCheck.h"

#include <filesystem>

#include <brion/target.h>

namespace bbploader
{
namespace
{
class InputGidSettingsChecker
{
public:
    static void check(const BBPLoaderParameters &input)
    {
        if (input.gids.has_value() && (*input.gids).empty())
        {
            throw std::invalid_argument("Cannot specify an empty list of GIDs");
        }

        if (input.targets.has_value() && (*input.targets).empty())
        {
            throw std::invalid_argument("Cannot specify an empty list of targets");
        }
    }
};

class TargetChecker
{
public:
    static void check(const brion::BlueConfig &config, const BBPLoaderParameters &input)
    {
        if (!input.targets.has_value() || (*input.targets).empty())
        {
            return;
        }

        const auto targetParsers = config.getTargets();
        for (const auto &trg : *input.targets)
        {
            if (trg.empty())
            {
                throw std::invalid_argument("Specified an empty target name");
            }

            bool exists = false;
            for (const auto &parser : targetParsers)
            {
                if (parser.contains(trg))
                {
                    exists = true;
                    break;
                }
            }
            if (!exists)
            {
                throw std::invalid_argument("Invalid or empty target: '" + trg + "'");
            }
        }
    }
};

class ReportChecker
{
public:
    static void check(const brion::BlueConfig &config, const BBPLoaderParameters &input)
    {
        const auto type = input.report_type;
        switch (type)
        {
        case ReportType::Spikes:
        {
            const auto uri = config.getSpikeSource();
            if (uri.getPath().empty() || !std::filesystem::exists(uri.getPath()))
            {
                throw std::invalid_argument("Unable to find Spike report file");
            }
            break;
        }
        case ReportType::Compartment:
        {
            if (input.report_name.empty())
            {
                throw std::invalid_argument("A compartment report was requested, but no report_name was provided");
            }

            const auto uri = config.getReportSource(input.report_name);
            if (uri.getPath().empty() || !std::filesystem::exists(uri.getPath()))
            {
                throw std::invalid_argument("Unable to find Voltage report file for '" + input.report_name + "'");
            }
            break;
        }
        case ReportType::None:
            break;
        }
    }
};

class NeuronSectionsChecker
{
public:
    static void check(const BBPLoaderParameters &input)
    {
        const auto &morphSettings = input.neuron_morphology_parameters;
        const auto loadSoma = morphSettings.load_soma;
        const auto loadAxon = morphSettings.load_axon;
        const auto loadDend = morphSettings.load_dendrites;
        if (!loadSoma && !loadAxon && !loadDend)
        {
            throw std::invalid_argument("All neuron sections cannot be disabled");
        }
    }
};
} // namespace

void ParameterCheck::checkInput(const brion::BlueConfig &config, const BBPLoaderParameters &input)
{
    InputGidSettingsChecker::check(input);
    TargetChecker::check(config, input);
    ReportChecker::check(config, input);
    NeuronSectionsChecker::check(input);
}
} // namespace bbploader
