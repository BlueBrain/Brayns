/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "GetMaterialIdsEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

namespace
{
class MaterialIdHelper
{
public:
    static std::vector<size_t> getIds(const std::map<size_t, brayns::MaterialPtr> &materials)
    {
        std::vector<size_t> ids;
        ids.reserve(materials.size());
        for (const auto &pair : materials)
        {
            auto id = pair.first;
            ids.push_back(id);
        }
        return ids;
    }
};
} // namespace

namespace brayns
{
std::string GetMaterialIdsEntrypoint::getName() const
{
    return "get-material-ids";
}

std::string GetMaterialIdsEntrypoint::getDescription() const
{
    return "Retreive the list of ID of the materials in given model";
}

void GetMaterialIdsEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &scene = getApi().getScene();
    auto &descriptor = ExtractModel::fromId(scene, params.id);
    auto &model = descriptor.getModel();
    auto &materials = model.getMaterials();
    GetMaterialIdsResult result;
    result.ids = MaterialIdHelper::getIds(materials);
    request.reply(result);
}
} // namespace brayns
