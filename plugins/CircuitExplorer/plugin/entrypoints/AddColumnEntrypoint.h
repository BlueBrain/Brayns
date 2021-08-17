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

#include <brayns/common/log.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <common/types.h>

#include <plugin/messages/AddColumnMessage.h>

class ColumnModel
{
public:
    static void add(brayns::Scene& scene, const AddColumnMessage& params)
    {
        // Create column model
        auto model = scene.createModel();

        // Color definition
        const brayns::Vector3f white = {1.f, 1.f, 1.f};

        // Material extra properties
        brayns::PropertyMap props;
        props.add({MATERIAL_PROPERTY_CAST_USER_DATA, false});
        props.add({MATERIAL_PROPERTY_SHADING_MODE,
                   static_cast<int>(MaterialShadingMode::diffuse)});
        props.add({MATERIAL_PROPERTY_CLIPPING_MODE,
                   static_cast<int>(MaterialClippingMode::no_clipping)});

        // Column material
        auto material = model->createMaterial(0, "column");
        material->setDiffuseColor(white);
        material->setProperties(props);

        // Bottom of the column
        const brayns::Vector3fs verticesBottom = {
            {-0.25f, -1.0f, -0.5f}, {0.25f, -1.0f, -0.5f},
            {0.5f, -1.0f, -0.25f},  {0.5f, -1.0f, 0.25f},
            {0.5f, -1.0f, -0.25f},  {0.5f, -1.0f, 0.25f},
            {0.25f, -1.0f, 0.5f},   {-0.25f, -1.0f, 0.5f},
            {-0.5f, -1.0f, 0.25f},  {-0.5f, -1.0f, -0.25f}};

        // Top of the column
        const brayns::Vector3fs verticesTop = {
            {-0.25f, 1.f, -0.5f}, {0.25f, 1.f, -0.5f}, {0.5f, 1.f, -0.25f},
            {0.5f, 1.f, 0.25f},   {0.5f, 1.f, -0.25f}, {0.5f, 1.f, 0.25f},
            {0.25f, 1.f, 0.5f},   {-0.25f, 1.f, 0.5f}, {-0.5f, 1.f, 0.25f},
            {-0.5f, 1.f, -0.25f}};

        // Extract column info
        const auto r = static_cast<float>(params.radius);

        // Bottom
        for (size_t i = 0; i < verticesBottom.size(); ++i)
        {
            model->addCylinder(0,
                               {verticesBottom[i],
                                verticesBottom[(i + 1) % verticesBottom.size()],
                                r / 2.f});
            model->addSphere(0, {verticesBottom[i], r});
        }

        // Top
        for (size_t i = 0; i < verticesTop.size(); ++i)
        {
            model->addCylinder(0, {verticesTop[i],
                                   verticesTop[(i + 1) % verticesTop.size()],
                                   r / 2.f});
            model->addSphere(0, {verticesTop[i], r});
        }

        // Middle
        for (size_t i = 0; i < verticesTop.size(); ++i)
            model->addCylinder(0, {verticesBottom[i], verticesTop[i], r / 2.f});

        // Register column model
        scene.addModel(
            std::make_shared<brayns::ModelDescriptor>(std::move(model),
                                                      "Column"));
    }
};

class AddColumnEntrypoint
    : public brayns::Entrypoint<AddColumnMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override { return "add-column"; }

    virtual std::string getDescription() const override
    {
        return "Add a visual 3D column as a cylinder to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        BRAYNS_INFO << "Building Column model.\n";
        ColumnModel::add(scene, params);
        request.reply(brayns::EmptyMessage());
    }
};