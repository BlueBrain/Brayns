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
#include <brayns/common/material/Material.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(MaterialManager)

namespace brayns
{
class MaterialManager : public BaseObject
{
public:
    MaterialManager();
    BRAYNS_API virtual ~MaterialManager();

    /**
     * Called after material-related changes have been made
     */
    BRAYNS_API virtual void commit() = 0;

    BRAYNS_API void clear();

    /**
        Adds a material object
        @param material Material object
        @return Index of the new material
    */
    BRAYNS_API size_t add(const Material& material);

    /**
        Replaces a material object
        @param material Material object
        @return Index of the material
    */
    BRAYNS_API void set(const size_t index, Material material);

    /**
        Removes a material object at a given index
        @param index Index of material to remove
    */
    BRAYNS_API void remove(const size_t index);
    /**
        Returns the material object for a given index
        @param index Index of material
        @return Reference to material object
    */
    BRAYNS_API Material& get(const size_t index);

    /**
        Checks the existence of a material. Raises an exception if the material
       is not registered
        @param index Index of material
    */
    BRAYNS_API void check(const size_t index);
    /**
        @return All materials
    */
    BRAYNS_API Materials& getMaterials() { return _materials; }
    BRAYNS_API size_t size() { return _materials.size(); }
    BRAYNS_API size_t addTexture(const std::string& filename);
    BRAYNS_API void removeTexture(const size_t id);

protected:
    Materials _materials;
    TexturesMap _textures;

    SERIALIZATION_FRIEND(MaterialManager)
};
}

#endif // MATERIALMANAGER_H
