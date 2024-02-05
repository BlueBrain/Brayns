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

#include "ParameterCheck.h"

#include <brion/target.h>

#include <brayns/utils/Filesystem.h>

namespace bbploader
{
namespace
{
void checkPercentage(const BBPLoaderParameters& input)
{
    if (input.percentage <= 0.f)
        throw std::invalid_argument(
            "BBPLoader: Cell percentage must be greater than 0.0");
    else if (input.percentage > 1.f)
        throw std::invalid_argument(
            "BBPLoader: Cell percentage must be cannot be greater than 1.0");
}

void checkTargets(const brion::BlueConfig& config,
                  const BBPLoaderParameters& input)
{
    if (input.targets.empty())
        return;

    const auto targetParsers = config.getTargets();
    for (const auto& trg : input.targets)
    {
        bool exists = false;
        for (const auto& parser : targetParsers)
        {
            if (parser.contains(trg))
            {
                exists = true;
                break;
            }
        }
        if (!exists)
            throw std::invalid_argument(
                "BBPLoader: Invalid or empty target: '" + trg + "'");
    }
}

void checkReport(const brion::BlueConfig& config,
                 const BBPLoaderParameters& input)
{
    switch (input.report_type)
    {
    case SimulationType::SPIKES:
    {
        const auto uri = config.getSpikeSource();
        if (uri.getPath().empty() || !fs::exists(uri.getPath()))
            throw std::invalid_argument(
                "BBPLoader: Unable to find Spike report file");
        break;
    }
    case SimulationType::COMPARTMENT:
    {
        if (input.report_name.empty())
            throw std::invalid_argument(
                "BBPLoader: A compartment report type was requested, but no "
                "report_name was provided");
        const auto uri = config.getReportSource(input.report_name);
        if (uri.getPath().empty() || !fs::exists(uri.getPath()))
            throw std::invalid_argument(
                "BBPLoader: Unable to find Voltage report file for '" +
                input.report_name + "'");
        break;
    }
    case SimulationType::NONE:
        break;
    }
}

void checkSpikeTransitionTime(const BBPLoaderParameters& input)
{
    if (input.spike_transition_time < 0.f)
        throw std::invalid_argument(
            "BBPLoader: spike_transition_time must be greater or equal to 0.0");
}

void checkRadiusModifications(const BBPLoaderParameters& input)
{
    if (input.neuron_morphology_parameters.radius_override < 0.f)
        throw std::invalid_argument(
            "BBPLoader: 'radius_override' cannot be negative");
    else if (input.neuron_morphology_parameters.radius_override == 0.f &&
             input.neuron_morphology_parameters.radius_multiplier <= 0.f)
        throw std::invalid_argument(
            "BBPLoader: 'radius_multiplier' cannot be less than or equal to "
            "0.0");
}

void checkNeuronSections(const BBPLoaderParameters& input)
{
    if (!input.neuron_morphology_parameters.load_soma &&
        !input.neuron_morphology_parameters.load_axon &&
        !input.neuron_morphology_parameters.load_dendrites)
        throw std::invalid_argument(
            "BBPLoader: All neuron sections cannot be disabled. Empty circuits "
            "are not allowed");
}
} // namespace

void ParameterCheck::checkInput(const brion::BlueConfig& config,
                                const BBPLoaderParameters& input)
{
    checkPercentage(input);
    checkTargets(config, input);
    checkReport(config, input);
    checkSpikeTransitionTime(input);
    checkRadiusModifications(input);
    checkNeuronSections(input);
}
} // namespace bbploader
