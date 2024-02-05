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

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIGetTargetsMessage.h>

class TargetRetriever
{
public:
    static std::vector<std::string> getTargets(const std::string& path)
    {
        std::vector<std::string> result;

        // Read config
        const brion::BlueConfig config(path);

        // Targets
        auto targets = config.getTargets();

        // Desired target types
        auto types = {brion::TargetType::TARGET_CELL,
                      brion::TargetType::TARGET_COMPARTMENT,
                      brion::TargetType::TARGET_ALL};

        // Extract target names
        for (const auto& target : targets)
        {
            for (const auto& type : types)
            {
                auto& names = target.getTargetNames(type);
                result.insert(result.end(), names.begin(), names.end());
            }
        }

        return result;
    }
};

class CIGetTargetsEntrypoint
    : public brayns::Entrypoint<CIGetTargetsParams, CIGetTargetsResult>
{
public:
    virtual std::string getName() const override { return "ci-get-targets"; }

    virtual std::string getDescription() const override
    {
        return "Return a list of targets from a circuit";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& path = params.path;
        CIGetTargetsResult result;
        result.targets = TargetRetriever::getTargets(path);
        request.reply(result);
    }
};