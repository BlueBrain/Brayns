/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{
class MaterialManager
{
public:
    MaterialManager();
    BRAYNS_API virtual ~MaterialManager();

    BRAYNS_API void clear();

    /**
        Set the material object for a given index.
        @param index Index of material
        @param material Material object
    */
    BRAYNS_API void set(const size_t index, const Material& material);

    /**
        Set a default material object for a given index, if the material does
       not already exists
        @param index Index of material
    */
    BRAYNS_API void set(const size_t index);
    /**
        Returns the material object for a given index
        @param index Index of material
        @return Reference to material object
    */
    BRAYNS_API Material& get(size_t index);

    /**
        @return All materials
    */
    BRAYNS_API Materials& getMaterials() { return _materials; }
    /**
        Returns the position of the material in the internal vector
        @param index Index of material
        @return Position of the material in the internal vector
    */
    BRAYNS_API size_t position(const size_t materialId);

private:
    Materials _materials;
    std::map<size_t, size_t> _materialMapping;
};
}

#endif // MATERIALMANAGER_H
