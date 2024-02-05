/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/network/messages/SimulationColorMessage.h>

class SimulationColorEntryPoint
    : public brayns::Entrypoint<SimulationColorMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-simulation-color";
    }

    virtual std::string getDescription() const override
    {
        return "Enables or disables the color of a given Circuit Model by its "
               "simulation values";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        auto& model = brayns::ExtractModel::fromId(scene, params.model_id);
        CircuitExplorerMaterial::setSimulationColorEnabled(model.getModel(),
                                                           params.enabled);
        request.reply(brayns::EmptyMessage());
    }
};
