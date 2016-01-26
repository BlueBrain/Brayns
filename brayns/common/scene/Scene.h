/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef SCENE_H
#define SCENE_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/TrianglesMesh.h>

namespace brayns
{

class Scene
{
public:
    BRAYNS_API Scene(RendererPtr renderer, GeometryParameters& geometryParameters);
    BRAYNS_API virtual ~Scene();

    BRAYNS_API virtual void commit() = 0;

    BRAYNS_API virtual void setMaterials(
        MaterialType materialType,
        size_t nbMaterials);

    BRAYNS_API MaterialPtr getMaterial(size_t index);

    BRAYNS_API virtual void loadData() = 0;

    BRAYNS_API virtual void buildGeometry() = 0;

    BRAYNS_API const Boxf& getWorldBounds() const { return _bounds; }

    BRAYNS_API void loadSWCFolder();
    BRAYNS_API void loadPDBFolder();
    BRAYNS_API void loadH5Folder();
    BRAYNS_API void loadMeshFolder();
    BRAYNS_API void buildEnvironment();

protected:
    // Parameters
    GeometryParameters& _geometryParameters;
    RendererPtr _renderer;

    // Model
    PrimitivesCollection _primitives;
    TrianglesMeshCollection _trianglesMeshes;
    Materials _materials;
    TexturesCollection _textures;

    Boxf _bounds;
};

}
#endif // SCENE_H
