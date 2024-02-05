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

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <io/DTILoader.h>

#include <messages/AddStreamlinesMessage.h>

namespace dti
{
class StreamlineBuilder
{
public:
    StreamlineBuilder(brayns::PluginAPI& api)
        : _api(&api)
    {
    }

    void addStreamlines(const AddStreamlinesMessage& params)
    {
        // Extract params
        auto& name = params.name;
        auto& gids = params.gids;
        auto& indices = params.indices;
        auto& vertices = params.vertices;
        auto opacity = params.opacity;
        auto radius = params.radius;
        auto colorScheme = params.color_scheme;

        // Extract API data
        auto& scene = _api->getScene();

        // Start loading model
        brayns::Log::info("Loading params <{}> from Json.", name);

        // Create model
        auto model = scene.createModel();

        // Create streamlines
        size_t nbStreamlines = 0;
        uint64_t startIndex = 0;
        for (size_t index = 0; index < indices.size(); ++index)
        {
            // Create material
            const auto materialId = gids.empty() ? 0 : gids[index];
            brayns::PropertyMap props;
            props.add({"shading_mode", 0});
            auto material =
                model->createMaterial(materialId, std::to_string(materialId),
                                      props);
            material->setOpacity(opacity);
            material->setSpecularColor({0, 0, 0});

            // Create streamline geometry
            const auto endIndex = indices[index];

            // Ignore streamlines with less than 2 points
            if (endIndex - startIndex < 2)
            {
                continue;
            }

            // Load points
            std::vector<brayns::Vector3f> points;
            std::vector<float> radii;
            brayns::Vector3f normal;
            for (uint64_t p = startIndex; p < endIndex; ++p)
            {
                const auto i = p * 3;
                const brayns::Vector3f point = {vertices[i], vertices[i + 1],
                                                vertices[i + 2]};
                points.push_back(point);
                radii.push_back(radius);
            }

            // Load colors from points
            const auto colors =
                DTILoader::getColorsFromPoints(points, opacity,
                                               ColorScheme(colorScheme));

            // Create streamlines
            brayns::Streamline streamline(points, colors, radii);
            model->addStreamline(materialId, streamline);
            startIndex = endIndex;

            ++nbStreamlines;
        }

        // Check if some streamlines created
        if (nbStreamlines == 0)
        {
            brayns::Log::info("No streamlines.");
            return;
        }

        // Register model
        auto descriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name);
        scene.addModel(descriptor);

        brayns::Log::info("{} streamlines loaded.", nbStreamlines);
    }

private:
    brayns::PluginAPI* _api;
};

class AddStreamlinesEntrypoint
    : public brayns::Entrypoint<AddStreamlinesMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override { return "add-streamlines"; }

    virtual std::string getDescription() const override
    {
        return "Add a streamline representation to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        StreamlineBuilder builder(getApi());
        builder.addStreamlines(params);
        triggerRender();
        request.reply(brayns::EmptyMessage());
    }
};
} // namespace dti
