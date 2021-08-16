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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/CircuitExplorerPlugin.h>
#include <plugin/adapters/MaterialAdapter.h>
#include <plugin/entrypoints/SetMaterialExtraAttributesEntrypoint.h>
#include <plugin/messages/TraceAnterogradeMessage.h>

class AnterogradeTracer
{
public:
    AnterogradeTracer(brayns::PluginAPI& api, CircuitExplorerPlugin& plugin)
        : _api(&api)
        , _plugin(&plugin)
    {
    }

    void trace(const TraceAnterogradeMessage& params)
    {
        // Validation
        if (params.cell_gids.empty())
        {
            throw brayns::EntrypointException("No input cell GIDs specified");
        }

        // Extract API data
        auto modelId = params.model_id;
        auto& engine = _api->getEngine();
        auto& scene = engine.getScene();
        auto& modelDescriptor = brayns::ExtractModel::fromId(scene, modelId);
        auto& model = modelDescriptor.getModel();
        auto& materials = model.getMaterials();

        // Retreive cell mapping
        auto cellMapper = _plugin->getMapperForCircuit(modelId);
        if (!cellMapper)
        {
            throw brayns::EntrypointException(
                "There is no cell mapping information for the given circuit");
        }
        const auto& cellMaterialMap = cellMapper->getMapping();

        // Cell GID -> Material ID
        auto getMaterialIds =
            [](const std::vector<uint32_t>& src,
               const std::unordered_map<size_t, MorphologyMap>& m)
        {
            std::unordered_set<int32_t> buffer;
            for (const auto& cellId : src)
            {
                auto morphologyMapIt = m.find(cellId);
                if (morphologyMapIt != m.end())
                {
                    const auto& morphologyMap = morphologyMapIt->second;
                    if (morphologyMap._hasMesh)
                        buffer.insert(
                            static_cast<int32_t>(morphologyMap._triangleIndx));
                    for (const auto& kvp : morphologyMap._coneMap)
                        buffer.insert(static_cast<int32_t>(kvp.first));
                    for (const auto& kvp : morphologyMap._sphereMap)
                        buffer.insert(static_cast<int32_t>(kvp.first));
                    for (const auto& kvp : morphologyMap._cylinderMap)
                        buffer.insert(static_cast<int32_t>(kvp.first));
                    for (const auto& kvp : morphologyMap._sdfGeometryMap)
                        buffer.insert(static_cast<int32_t>(kvp.first));
                }
            }
            return buffer;
        };

        // Gather material ids for the source and target cells
        auto sourceCellMaterialIds =
            getMaterialIds(params.cell_gids, cellMaterialMap);
        auto targetCellMaterialIds =
            getMaterialIds(params.target_cell_gids, cellMaterialMap);

        // Enable extra attributes on materials
        brayns::SetMaterialExtraAttributes::toModel(modelDescriptor);

        // Reset all cells to non-stained color
        auto& cellColor = params.non_connected_cells_color;
        for (const auto& pair : materials)
        {
            brayns::ExtendedMaterial material(*pair.second);
            material.setDiffuseColor(cellColor);
            material.setSpecularColor(brayns::Vector3d(1.0));
            material.setSpecularExponent(20.0);
            material.setEmission(0.0);
            material.setGlossiness(1.0);
            material.setReflectionIndex(0.0);
            material.setRefractionIndex(0.0);
            material.setOpacity(std::min(std::max(cellColor.a, 0.1), 1.0));
            material.setShadingMode(MaterialShadingMode::diffuse);
            material.setClippingMode(MaterialClippingMode::no_clipping);
            material.setSimulationDataCast(false);
        }

        // Stain (if any) source cell
        auto& sourceColor = params.source_cell_color;
        for (const auto& materialId : sourceCellMaterialIds)
        {
            brayns::ExtendedMaterial material(
                brayns::ExtractMaterial::fromId(modelDescriptor, materialId));
            material.setDiffuseColor(sourceColor);
            material.setSpecularColor(brayns::Vector3d(1.0));
            material.setSpecularExponent(20.0);
            material.setEmission(0.0);
            material.setGlossiness(1.0);
            material.setReflectionIndex(0.0);
            material.setRefractionIndex(0.0);
            material.setOpacity(1.0);
            material.setShadingMode(MaterialShadingMode::diffuse);
            material.setClippingMode(MaterialClippingMode::no_clipping);
            material.setSimulationDataCast(false);
        }

        // Stain target cells
        auto& targetColor = params.connected_cells_color;
        for (const auto& materialId : sourceCellMaterialIds)
        {
            brayns::ExtendedMaterial material(
                brayns::ExtractMaterial::fromId(modelDescriptor, materialId));
            material.setDiffuseColor(targetColor);
            material.setSpecularColor(brayns::Vector3d(1.0));
            material.setSpecularExponent(20.0);
            material.setEmission(0.0);
            material.setGlossiness(1.0);
            material.setReflectionIndex(0.0);
            material.setRefractionIndex(0.0);
            material.setOpacity(1.0);
            material.setShadingMode(MaterialShadingMode::diffuse);
            material.setClippingMode(MaterialClippingMode::no_clipping);
            material.setSimulationDataCast(false);
        }

        // Mark everything modified
        modelDescriptor.markModified();
        scene.markModified();
        engine.triggerRender();
    }

private:
    brayns::PluginAPI* _api;
    CircuitExplorerPlugin* _plugin;
};

class TraceAnterogradeEntrypoint
    : public brayns::Entrypoint<TraceAnterogradeMessage, brayns::EmptyMessage>
{
public:
    TraceAnterogradeEntrypoint(CircuitExplorerPlugin& plugin)
        : _plugin(&plugin)
    {
    }

    virtual std::string getName() const override { return "trace-anterograde"; }

    virtual std::string getDescription() const override
    {
        return "Performs neuronal tracing showing efferent and afferent "
               "synapse relationship between cells (including projections)";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        AnterogradeTracer tracer(getApi(), *_plugin);
        tracer.trace(params);
        request.reply(brayns::EmptyMessage());
    }

private:
    CircuitExplorerPlugin* _plugin;
};