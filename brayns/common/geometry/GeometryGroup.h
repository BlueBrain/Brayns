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

SERIALIZATION_ACCESS(GroupTransformation)
SERIALIZATION_ACCESS(GeometryGroup)
SERIALIZATION_ACCESS(GroupAttributes)

namespace brayns
{
struct GroupTransformation : public BaseObject
{
    GroupTransformation() {}
    GroupTransformation(const GroupTransformation& rhs);
    GroupTransformation& operator=(const GroupTransformation& rhs);

    Vector3f& translation() { return _translation; }
    void translation(const Vector3f& value)
    {
        _updateValue(_translation, value);
    }
    Vector3f& scale() { return _scale; }
    void scale(const Vector3f& value) { _updateValue(_scale, value); }
    Vector3f& rotation() { return _rotation; }
    void rotation(const Vector3f& value) { _updateValue(_rotation, value); }
private:
    Vector3f _translation{0, 0, 0};
    Vector3f _scale{1, 1, 1};
    Vector3f _rotation{0, 0, 0};

    SERIALIZATION_FRIEND(GroupTransformation)
};

struct GroupAttributes : public BaseObject
{
    GroupAttributes() {}
    GroupAttributes(const GroupAttributes& rhs);
    GroupAttributes& operator=(const GroupAttributes& rhs);

    GroupAttributes(const std::string& name, const std::string& uri,
                    const bool enabled)
        : _name(name)
        , _uri(uri)
        , _enabled(enabled)
        , _visible(true)
        , _boundingBox(true)
    {
        _transformations.push_back(GroupTransformation());
    }

    bool enabled() const { return _enabled; }
    bool visible() const { return _visible; }
    bool boundingBox() const { return _boundingBox; }
    GroupTransformations& transformations() { return _transformations; }
private:
    std::string _name;
    std::string _uri;
    bool _enabled;
    bool _visible;
    bool _boundingBox;
    GroupTransformations _transformations;

    SERIALIZATION_FRIEND(GroupAttributes)
};

class GeometryGroup : public BaseObject
{
public:
    BRAYNS_API GeometryGroup();
    BRAYNS_API GeometryGroup(MaterialManagerPtr materialManager);
    BRAYNS_API GeometryGroup(const GeometryGroup& rhs);
    BRAYNS_API GeometryGroup& operator=(const GeometryGroup& rhs);

    BRAYNS_API virtual ~GeometryGroup();

    /** Unloads geometry, materials, lights, models, etc. to free memory. */
    BRAYNS_API virtual void unload();

    /**
        Return true if the geometry group does not contain any geometry. False
       otherwize
    */
    BRAYNS_API bool empty() const;

    /**
        Return true if the geometry group is dirty, false otherwize
    */
    BRAYNS_API bool dirty() const;

    /**
        Returns the bounding box for the whole scene
    */
    Boxf& getBounds();
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
    BRAYNS_API void setSpheresDirty(const bool value)
    {
        _updateValue(_spheresDirty, value);
    }
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
        _updateValue(_cylindersDirty, value);
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
    BRAYNS_API void setConesDirty(const bool value)
    {
        _updateValue(_conesDirty, value);
    }
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
        _updateValue(_trianglesMeshesDirty, value);
    }
    BRAYNS_API bool trianglesMeshesDirty() { return _trianglesMeshesDirty; }
    BRAYNS_API void logInformation();

    BRAYNS_API bool useSimulationModel() const { return _useSimulationModel; }
    BRAYNS_API void useSimulationModel(const bool value)
    {
        _updateValue(_useSimulationModel, value);
    }

protected:
    MaterialManagerPtr _materialManager;
    SpheresMap _spheres;
    bool _spheresDirty{true};
    CylindersMap _cylinders;
    bool _cylindersDirty{true};
    ConesMap _cones;
    bool _conesDirty{true};
    TrianglesMeshMap _trianglesMeshes;
    bool _trianglesMeshesDirty{true};
    Boxf _bounds;
    bool _useSimulationModel{false};

    SERIALIZATION_FRIEND(GeometryGroup)
};
}
#endif // GEOMETRYGROUP_H
