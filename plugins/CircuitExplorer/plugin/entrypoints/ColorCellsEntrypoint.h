/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <plugin/CircuitExplorerPlugin.h>
#include <plugin/messages/ColorCellsMessage.h>

class CellColorizer
{
public:
    CellColorizer(brayns::PluginAPI& api, CircuitExplorerPlugin& plugin)
        : _api(&api)
        , _plugin(&plugin)
    {
    }

    void colorCells(const ColorCellsMessage& params)
    {
        // Extract message data
        auto modelId = params.model_id;
        auto& gids = params.gids;
        auto& colors = params.colors;

        // Check message consistency
        if (gids.size() * 3 != colors.size())
        {
            throw brayns::EntrypointException(
                1, "There must be 3 color components for each GID");
        }

        // Extract API data
        auto& engine = _api->getEngine();
        auto& scene = _api->getScene();

        // Extract model
        auto& model = brayns::ExtractModel::fromId(scene, modelId);

        // Get cell mapper
        auto mapper = _plugin->getMapperForCircuit(modelId);
        if (!mapper)
        {
            return;
        }

        // Helper to apply color to material
        auto setMaterialColor = [](brayns::ModelDescriptor& model,
                                   size_t materialId,
                                   const brayns::Vector3d& color)
        {
            auto material = model.getModel().getMaterial(materialId);
            if (!material)
            {
                return;
            }
            material->setDiffuseColor(color);
            material->markModified();
            material->commit();
        };

        // Batch GIDs with colors
        std::vector<std::vector<uint64_t>> gidBatches(gids.size());
        std::vector<brayns::Vector3d> gidColors(gids.size());

        // Parse GIDs ranges
        for (size_t j = 0; j < gids.size(); ++j)
        {
            // Split at comma to find sub-ranges
            auto tokens = brayns::string_utils::split(gids[j], ',');

            // Parse sub-ranges
            auto& batchGids = gidBatches[j];
            for (const auto& code : tokens)
            {
                // Find dash if code is a range of GIDs
                auto dash = code.find('-');

                // Single value
                if (dash == std::string::npos)
                {
                    batchGids.push_back(std::stoul(code));
                    continue;
                }

                // Range first-last
                auto first = std::stoul(code.substr(0, dash));
                auto last = std::stoul(code.substr(dash + 1));
                for (uint64_t i = first; i <= last; ++i)
                {
                    batchGids.push_back(i);
                }
            }

            // Add color for the range in the same index
            auto colorIndex = j * 3;
            gidColors[j] = {colors[colorIndex], colors[colorIndex + 1],
                            colors[colorIndex + 2]};
        }

        // This should not happen
        if (gidColors.size() != gidBatches.size())
        {
            throw brayns::EntrypointException(
                9, "Internal error during parsing of cells to color");
        }

        // Color cells
        const auto& mapping = mapper->getMapping();
        for (size_t i = 0; i < gidBatches.size(); ++i)
        {
            auto& gidBatch = gidBatches[i];
            auto& color = gidColors[i];

            for (const auto cellGID : gidBatch)
            {
                auto it = mapping.find(cellGID);
                if (it != mapping.end())
                {
                    const MorphologyMap& mmap = it->second;
                    for (const auto& kv : mmap._coneMap)
                        setMaterialColor(model, kv.first, color);
                    for (const auto& kv : mmap._cylinderMap)
                        setMaterialColor(model, kv.first, color);
                    for (const auto& kv : mmap._sdfBezierMap)
                        setMaterialColor(model, kv.first, color);
                    for (const auto& kv : mmap._sdfGeometryMap)
                        setMaterialColor(model, kv.first, color);
                    for (const auto& kv : mmap._sphereMap)
                        setMaterialColor(model, kv.first, color);
                }
            }
        }

        // Commit
        model.markModified();
        scene.markModified();
        engine.triggerRender();
    }

private:
    brayns::PluginAPI* _api;
    CircuitExplorerPlugin* _plugin;
};

class ColorCellsEntrypoint
    : public brayns::Entrypoint<ColorCellsMessage, brayns::EmptyMessage>
{
public:
    ColorCellsEntrypoint(CircuitExplorerPlugin& plugin)
        : _plugin(&plugin)
    {
    }

    virtual std::string getName() const override { return "color-cells"; }

    virtual std::string getDescription() const override
    {
        return "Color cells with given colors using their GID";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        CellColorizer colorizer(getApi(), *_plugin);
        colorizer.colorCells(params);
        request.reply(brayns::EmptyMessage());
    }

private:
    CircuitExplorerPlugin* _plugin;
};