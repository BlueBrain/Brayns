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

#ifndef GEOMETRYGROUP_H
#define GEOMETRYGROUP_H

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/TrianglesMesh.h>
#include <brayns/common/material/MaterialManager.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(GeometryGroup)

namespace brayns
{
class GeometryGroup : public BaseObject
{
public:
    GeometryGroup();
    BRAYNS_API virtual ~GeometryGroup();

    /** Unloads geometry, materials, lights, models, etc. to free memory. */
    BRAYNS_API void unload();

    /**
        Return true if the geometry group does not contain any geometry. False
       otherwize
    */
    BRAYNS_API bool empty() const;

    /**
        Sets enability of the geometry group. If disabled, the group is not
       rendered
      */
    BRAYNS_API bool enabled() { return _enabled; }
    BRAYNS_API void enable() { _enabled = true; }
    BRAYNS_API void disable() { _enabled = false; }
    BRAYNS_API bool dirty();
    /**
        Returns the bounding box for the whole scene
    */
    Boxf& getBounds() { return _bounds; }
    /**
        Returns spheres handled by the geometry group
    */
    BRAYNS_API SpheresMap& getSpheres() { return _spheres; }
    /**
      Replaces a sphere in the scene
      @param materialId Material of the sphere
      @param index Index of the sphere in the scene, for the given material
      @param sphere New sphere

      */
    BRAYNS_API void setSphere(const size_t materialId, const uint64_t index,
                              const Sphere& sphere);

    /**
     * @brief Sets spheres as dirty, meaning that they need to be serialized
     *        and sent to the rendering engine
     */
    BRAYNS_API void setSpheresDirty(const bool value) { _spheresDirty = value; }
    BRAYNS_API bool spheresDirty() { return _spheresDirty; }
    /**
        Returns cylinders handled by the scene
    */
    /**
      Adds a sphere to the geometry group
      @param materialId Material of the sphere
      @param sphere Sphere to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addSphere(const size_t materialId,
                                  const Sphere& sphere);

    /**
        Returns cylinders handled by the geometry group
      */
    BRAYNS_API CylindersMap& getCylinders() { return _cylinders; }
    /**
      Replaces a cylinder in the scene
      @param materialId Material of the cylinder
      @param index Index of the cylinder in the scene, for the given material
      @param cylinder New cylinder

      */
    BRAYNS_API void setCylinder(const size_t materialId, const uint64_t index,
                                const Cylinder& cylinder);
    /**
      Adds a cylinder to the scene
      @param materialId Material of the cylinder
      @param cylinder Cylinder to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addCylinder(const size_t materialId,
                                    const Cylinder& cylinder);
    /**
     * @brief Sets cylinders as dirty, meaning that they need to be serialized
     *        and sent to the rendering engine
     */
    BRAYNS_API void setCylindersDirty(const bool value)
    {
        _cylindersDirty = value;
    }
    BRAYNS_API bool cylindersDirty() { return _cylindersDirty; }
    /**
        Returns cones handled by the geometry group
    */
    BRAYNS_API ConesMap& getCones() { return _cones; }
    /**
      Replaces a cone in the scene
      @param materialId Material of the cone
      @param index Index of the cone in the scene, for the given material
      @param cone New sphere

      */
    BRAYNS_API void setCone(const size_t materialId, const uint64_t index,
                            const Cone& cone);
    /**
      Adds a cone to the scene
      @param materialId Material of the cone
      @param cone Cone to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addCone(const size_t materialId, const Cone& cone);
    /**
     * @brief Sets cones as dirty, meaning that they need to be serialized
     *        and sent to the rendering engine
     */
    BRAYNS_API void setConesDirty(const bool value) { _conesDirty = value; }
    BRAYNS_API bool conesDirty() { return _conesDirty; }
    /**
        Returns triangle meshes handled by the geometry group
    */
    BRAYNS_API TrianglesMeshMap& getTrianglesMeshes()
    {
        return _trianglesMeshes;
    }
    /**
     * @brief Sets cones as dirty, meaning that they need to be serialized
     *        and sent to the rendering engine
     */
    BRAYNS_API void setTrianglesMeshesDirty(const bool value)
    {
        _trianglesMeshesDirty = value;
    }
    BRAYNS_API bool trianglesMeshesDirty() { return _trianglesMeshesDirty; }
    /**
     * @return The material manager used by the geometry group
     */
    MaterialManager& getMaterialManager() { return _materialManager; }
private:
    MaterialManager _materialManager;
    SpheresMap _spheres;
    bool _spheresDirty{true};
    CylindersMap _cylinders;
    bool _cylindersDirty{true};
    ConesMap _cones;
    bool _conesDirty{true};
    TrianglesMeshMap _trianglesMeshes;
    bool _trianglesMeshesDirty{true};
    Boxf _bounds;
    bool _enabled{true};

    SERIALIZATION_FRIEND(GeometryGroup)
};
}
#endif // GEOMETRYGROUP_H
