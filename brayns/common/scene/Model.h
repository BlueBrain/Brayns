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

#ifndef Model_H
#define Model_H

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/TrianglesMesh.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Model)
SERIALIZATION_ACCESS(ModelDescriptor)

namespace brayns
{
/**
 * @brief The ModelDescriptor struct defines the metadata attached to a model.
 * Model descriptor are exposed via the HTTP/WS interface.
 * - Enabling a model means that the model is part of scene. If disabled, the
 * model still exists in Brayns, but is removed from the rendered scene.
 * - The visible attribute defines if the model should be visible or not. If
 * invisible, the model is removed from the BVH.
 * - If set to true, the bounding box attribute displays a bounding box for the
 * current model
 */
struct ModelDescriptor : public BaseObject
{
    ModelDescriptor() = default;
    ModelDescriptor(ModelDescriptor&& rhs) = default;
    ModelDescriptor& operator=(ModelDescriptor&& rhs) = default;

    ModelDescriptor(const std::string& name, const std::string& path,
                    const ModelMetadata& metadata, ModelPtr model);

    bool getEnabled() const { return _visible || _boundingBox; }
    bool getVisible() const { return _visible; }
    bool getBoundingBox() const { return _boundingBox; }
    const Transformations& getTransformations() const
    {
        return _transformations;
    }
    const ModelMetadata& getMetadata() const { return _metadata; }
    const std::string& getName() const { return _name; }
    const std::string& getPath() const { return _path; }
    const Model& getModel() const { return *_model; }
    Model& getModel() { return *_model; }
private:
    std::string _name;
    std::string _path;
    ModelMetadata _metadata;
    bool _visible{true};
    bool _boundingBox{false};
    Transformations _transformations;
    ModelPtr _model;

    SERIALIZATION_FRIEND(ModelDescriptor)
};

/**
 * @brief The asbtract Model class holds the geometry attached to an asset of
 * the scene (mesh, circuit, volume, etc). The model handles resources attached
 * to the geometry such as implementation specific classes, and acceleration
 * structures). Models provide a simple API to manipulate primitives (spheres,
 * cylinders, triangle meshes, etc).
 */
class Model
{
public:
    BRAYNS_API Model() = default;

    BRAYNS_API virtual ~Model() = default;

    /**
        Return true if the geometry Model does not contain any geometry. False
       otherwize
    */
    BRAYNS_API bool empty() const;

    /**
        Return true if the geometry Model is dirty, false otherwize
    */
    BRAYNS_API bool dirty() const;

    /**
        Returns the bounds for the Model
    */
    const Boxf& getBounds() const { return _bounds; }
    Boxf& getBounds() { return _bounds; }
    /**
        Returns spheres handled by the Model
    */
    BRAYNS_API SpheresMap& getSpheres() { return _spheres; }
    /**
      Adds a sphere to the model
      @param materialId Id of the material for the sphere
      @param sphere Sphere to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addSphere(const size_t materialId,
                                  const Sphere& sphere);

    /**
        Returns cylinders handled by the model
      */
    BRAYNS_API CylindersMap& getCylinders() { return _cylinders; }
    /**
      Adds a cylinder to the model
      @param materialId Id of the material for the cylinder
      @param cylinder Cylinder to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addCylinder(const size_t materialId,
                                    const Cylinder& cylinder);
    /**
        Returns cones handled by the model
    */
    BRAYNS_API ConesMap& getCones() { return _cones; }
    /**
      Adds a cone to the model
      @param materialId Id of the material for thecone
      @param cone Cone to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addCone(const size_t materialId, const Cone& cone);
    /**
        Returns triangle meshes handled by the model
    */
    BRAYNS_API TrianglesMeshMap& getTrianglesMeshes()
    {
        return _trianglesMeshes;
    }

    /**
     * @brief logInformation Logs information about the model, like the number
     * of primitives, and the associated memory footprint.
     */
    BRAYNS_API void logInformation();

    BRAYNS_API void useSimulationModel(const bool value)
    {
        _useSimulationModel = value;
    }
    BRAYNS_API bool getUseSimulationModel() const
    {
        return _useSimulationModel;
    }
    /**
        Sets the materials handled by the model, and available to the geometry
        @param colorMap Specifies the algorithm that is used to create the
       materials. For instance MT_RANDOM creates materials with random colors,
       transparency, reflection, and light emission
    */
    void BRAYNS_API setMaterialsColorMap(const MaterialsColorMap colorMap);

    /** Factory method to create an engine-specific material. */
    BRAYNS_API virtual MaterialPtr createMaterial(const size_t materialId,
                                                  const std::string& name) = 0;
    /**
     * @brief createMissingMaterials Checks that all materials exist for
     * existing geometry in the model. Missing materials are created with the
     * default parameters
     */
    BRAYNS_API void createMissingMaterials();

    /**
     * @brief getMaterials Returns a reference to the map of materials handled
     * by the model
     * @return The map of materials handled by the model
     */
    BRAYNS_API MaterialMap& getMaterials() { return _materials; }
    BRAYNS_API const MaterialMap& getMaterials() const { return _materials; }
    /**
     * @brief getMaterial Returns a pointer to a specific material
     * @param materialId Id of the material
     * @return A pointer to the material or an exception if the material is not
     * registered in the model
     */
    BRAYNS_API MaterialPtr getMaterial(const size_t materialId) const;

protected:
    MaterialMap _materials;
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

    SERIALIZATION_FRIEND(Model)
};
}
#endif // Model_H
