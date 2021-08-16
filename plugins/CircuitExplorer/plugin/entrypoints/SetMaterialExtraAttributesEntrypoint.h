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

#include <plugin/messages/GetMaterialIdsMessage.h>

namespace brayns
{
class SetMaterialExtraAttributes
{
public:
    static void toModel(Scene& scene, size_t modelId)
    {
        auto& descriptor = ExtractModel::fromId(scene, modelId);
        toModel(descriptor);
    }

    static void toModel(ModelDescriptor& descriptor)
    {
        auto& model = descriptor.getModel();
        toModel(model);
    }

    static void toModel(Model& model)
    {
        static const auto properties = _getExtraAttributes();
        for (const auto& pair : model.getMaterials())
        {
            toMaterial(*pair.second);
        }
    }

    static void toMaterial(Material& material)
    {
        auto& properties = _getExtraAttributes();
        material.updateProperties(properties);
    }

private:
    static const PropertyMap& _getExtraAttributes()
    {
        static const auto properties = _createExtraAttributes();
        return properties;
    }

    static PropertyMap _createExtraAttributes()
    {
        PropertyMap properties;
        properties.add({MATERIAL_PROPERTY_CAST_USER_DATA, false});
        properties.add({MATERIAL_PROPERTY_SHADING_MODE,
                        int(MaterialShadingMode::diffuse)});
        properties.add({MATERIAL_PROPERTY_CLIPPING_MODE,
                        int(MaterialClippingMode::no_clipping)});
        properties.add({MATERIAL_PROPERTY_USER_PARAMETER, 1.0});
        return properties;
    }
};

class SetMaterialExtraAttributesEntrypoint
    : public Entrypoint<GetMaterialIdsParams, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-material-extra-attributes";
    }

    virtual std::string getDescription() const override
    {
        return "Add extra material attributes necessary for the Circuit "
               "Explorer renderer";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto& scene = getApi().getScene();
        SetMaterialExtraAttributes::toModel(scene, modelId);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns