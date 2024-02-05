/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/utils/Filesystem.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIInfoMessage.h>

class CircuitInfoRetriever
{
public:
    static CIInfoResult getCircuitInfo(const std::string& path)
    {
        // Validate path
        if (!fs::exists(path))
        {
            throw brayns::EntrypointException(9, "Circuit not found");
        }

        // Result
        CIInfoResult result;

        // Hard-coded properties
        result.cells_properties = {"etype", "mtype",    "morphology_class",
                                   "layer", "position", "orientation"};

        // Open circuit
        const brion::BlueConfig config(path);
        const brain::Circuit circuit(config);

        // Extract info
        result.cells_count = circuit.getNumNeurons();
        result.e_types = circuit.getElectrophysiologyTypeNames();
        result.m_types = circuit.getMorphologyTypeNames();
        result.reports = config.getSectionNames(brion::CONFIGSECTION_REPORT);
        result.spike_report = config.getSpikeSource().getPath();

        // All target types
        auto types = {brion::TargetType::TARGET_CELL,
                      brion::TargetType::TARGET_COMPARTMENT,
                      brion::TargetType::TARGET_ALL};

        // Extract targets
        for (const auto& target : config.getTargets())
        {
            for (const auto& type : types)
            {
                auto& names = target.getTargetNames(type);
                result.targets.insert(result.targets.end(), names.begin(),
                                      names.end());
            }
        }

        // Success
        return result;
    }
};

class CIInfoEntrypoint : public brayns::Entrypoint<CIInfoParams, CIInfoResult>
{
public:
    virtual std::string getName() const override { return "ci-info"; }

    virtual std::string getDescription() const override
    {
        return "Return general info about a circuit";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& path = params.path;
        auto result = CircuitInfoRetriever::getCircuitInfo(path);
        request.reply(result);
    }
};
