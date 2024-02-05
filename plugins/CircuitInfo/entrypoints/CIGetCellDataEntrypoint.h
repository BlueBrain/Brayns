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

#include <messages/CIGetCellDataMessage.h>

class CellDataRetriever
{
public:
    static CIGetCellDataResult getCellData(const CIGetCellDataParams& params)
    {
        // Result
        CIGetCellDataResult result;

        // Load data
        const brion::BlueConfig config(params.path);
        const brain::Circuit circuit(config);
        const brion::GIDSet gids(params.ids.begin(), params.ids.end());

        // Electrical types
        if (_requires(params, "etype"))
        {
            auto etypeNames = circuit.getElectrophysiologyTypeNames();
            auto etypeIndices = circuit.getElectrophysiologyTypes(gids);
            result.etypes.reserve(etypeIndices.size());
            for (const auto index : etypeIndices)
                result.etypes.push_back(etypeNames[index]);
        }

        // Morphology types
        if (_requires(params, "mtype"))
        {
            auto mtypeNames = circuit.getMorphologyTypeNames();
            auto mtypeIndices = circuit.getMorphologyTypes(gids);
            result.mtypes.reserve(mtypeIndices.size());
            for (const auto index : mtypeIndices)
                result.mtypes.push_back(mtypeNames[index]);
        }

        // Morphology classes
        if (_requires(params, "morphology_class"))
            result.morphology_classes = circuit.getMorphologyNames(gids);

        // Layers
        if (_requires(params, "layer"))
        {
            const auto& tsvFile =
                config.get(brion::BlueConfigSection::CONFIGSECTION_RUN,
                           "Default", "MEComboInfoFile");
            result.layers = circuit.getLayers(gids, tsvFile);
        }

        // Positions
        if (_requires(params, "position"))
        {
            auto positions = circuit.getPositions(gids);
            result.positions.reserve(positions.size() * 3);
            for (const auto& pos : positions)
            {
                result.positions.push_back(pos.x);
                result.positions.push_back(pos.y);
                result.positions.push_back(pos.z);
            }
        }

        // Orientations
        if (_requires(params, "orientation"))
        {
            auto orientations = circuit.getRotations(gids);
            result.orientations.reserve(orientations.size() * 4);
            for (const auto& o : orientations)
            {
                result.orientations.push_back(o.w);
                result.orientations.push_back(o.x);
                result.orientations.push_back(o.y);
                result.orientations.push_back(o.z);
            }
        }

        // Success
        return result;
    }

private:
    static bool _requires(const CIGetCellDataParams& params,
                          const std::string& property)
    {
        auto& properties = params.properties;
        auto first = properties.begin();
        auto last = properties.end();
        auto i = std::find(first, last, property);
        return i != last;
    }
};

class CIGetCellDataEntrypoint
    : public brayns::Entrypoint<CIGetCellDataParams, CIGetCellDataResult>
{
public:
    virtual std::string getName() const override { return "ci-get-cell-data"; }

    virtual std::string getDescription() const override
    {
        return "Return data attached to one or many cells";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto result = CellDataRetriever::getCellData(params);
        request.reply(result);
    }
};