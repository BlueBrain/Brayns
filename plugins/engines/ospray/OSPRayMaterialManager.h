/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef OSPRAYMATERIALMANAGER_H
#define OSPRAYMATERIALMANAGER_H

#include <brayns/common/material/MaterialManager.h>
#include <ospray_cpp/Data.h>

namespace brayns
{
class OSPRayMaterialManager : public MaterialManager
{
public:
    OSPRayMaterialManager(const uint32_t flags);

    void commit() final;
    OSPMaterial getOSPMaterial(const size_t index);
    OSPData getData() { return _ospMaterialData; }
private:
    void _commitMaterial(OSPMaterial ospMaterial, Material& material);
    OSPTexture2D _createTexture2D(const size_t id);
    std::vector<OSPMaterial> _ospMaterials;
    OSPData _ospMaterialData{nullptr};
    std::map<size_t, OSPTexture2D> _ospTextures;
    size_t _memoryManagementFlags{OSP_DATA_SHARED_BUFFER};
};
}

#endif // OSPRAYMATERIALMANAGER_H
