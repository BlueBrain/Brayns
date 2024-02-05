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

#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/SetSpikeSimulationFromFileMessage.h>

#include <DTIPlugin.h>

namespace dti
{
class SetSpikeSimulationFromFileEntrypoint
    : public brayns::Entrypoint<SetSpikeSimulationFromFileMessage,
                                brayns::EmptyMessage>
{
public:
    SetSpikeSimulationFromFileEntrypoint(DTIPlugin& plugin)
        : _plugin(&plugin)
    {
    }

    virtual std::string getName() const override
    {
        return "set-spike-simulation-from-file";
    }

    virtual std::string getDescription() const override
    {
        return "Add a spike simulation loaded from a file to a model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        _plugin->updateSpikeSimulationFromFile(params);
        triggerRender();
        request.reply(brayns::EmptyMessage());
    }

private:
    DTIPlugin* _plugin;
};
} // namespace dti