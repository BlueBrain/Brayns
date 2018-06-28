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
#include <brayns/common/Transformation.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/Streamlines.h>
#include <brayns/common/geometry/TrianglesMesh.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Model)
SERIALIZATION_ACCESS(ModelParams)
SERIALIZATION_ACCESS(ModelDescriptor)
SERIALIZATION_ACCESS(ModelInstance)

namespace brayns
{
class ModelInstance : public BaseObject
{
public:
    ModelInstance() = default;
    ModelInstance(const bool visible, const bool boundingBox,
                  const Transformation& transformation)
        : _visible(visible)
        , _boundingBox(boundingBox)
        , _transformation(transformation)
    {
    }
    bool getVisible() const { return _visible; }
    void setVisible(const bool visible) { _updateValue(_visible, visible); }
    bool getBoundingBox() const { return _boundingBox; }
    void setBoundingBox(const bool enabled)
    {
        _updateValue(_boundingBox, enabled);
    }
    const Transformation& getTransformation() const { return _transformation; }
    void setTransformation(const Transformation& transformation)
    {
        _updateValue(_transformation, transformation);
    }

    void setModelID(const size_t id) { _updateValue(_modelID, id); }
    size_t getModelID() const { return _modelID; }
    void setInstanceID(const size_t id) { _updateValue(_instanceID, id); }
    size_t getInstanceID() const { return _instanceID; }
protected:
    size_t _modelID{0};
    size_t _instanceID{0};
    bool _visible{true};
    bool _boundingBox{false};
    Transformation _transformation;

    SERIALIZATION_FRIEND(ModelInstance)
};

class ModelParams : public ModelInstance
{
public:
    ModelParams() = default;

    ModelParams(const std::string& path);
    ModelParams(const std::string& name, const std::string& path);

    ModelParams(ModelParams&& rhs) = default;
    ModelParams& operator=(ModelParams&& rhs) = default;

    ModelParams(const ModelParams& rhs) = default;
    ModelParams& operator=(const ModelParams& rhs) = default;

    void setName(const std::string& name) { _updateValue(_name, name); }
    const std::string& getName() const { return _name; }
    void setPath(const std::string& path) { _updateValue(_path, path); }
    const std::string& getPath() const { return _path; }
protected:
    std::string _name;
    std::string _path;

    SERIALIZATION_FRIEND(ModelParams)
};

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
class ModelDescriptor : public ModelParams
{
public:
    ModelDescriptor() = default;

    ModelDescriptor(ModelDescriptor&& rhs) = default;
    ModelDescriptor& operator=(ModelDescriptor&& rhs) = default;

    ModelDescriptor(ModelPtr model, const std::string& path);
    ModelDescriptor(ModelPtr model, const std::string& path,
                    const ModelMetadata& metadata);
    ModelDescriptor(ModelPtr model, const std::string& name,
                    const std::string& path, const ModelMetadata& metadata);

    ModelDescriptor& operator=(const ModelParams& rhs);

    bool getEnabled() const { return _visible || _boundingBox; }
    const ModelMetadata& getMetadata() const { return _metadata; }
    const Model& getModel() const { return *_model; }
    Model& getModel() { return *_model; }
    void addInstance(const ModelInstance& instance);
    void removeInstance(const size_t id);
    ModelInstance* getInstance(const size_t id);
    const ModelInstances& getInstances() const { return _instances; }
    Boxf getInstancesBounds() const;

private:
    size_t _nextInstanceID{0};
    Boxf _bounds;
    ModelMetadata _metadata;
    ModelPtr _model;
    ModelInstances _instances;

    SERIALIZATION_FRIEND(ModelDescriptor)
};

/**
 * The abstract Model class holds the geometry attached to an asset of
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

    virtual void commit() = 0;

    /**
     * @return true if the geometry Model does not contain any geometry, false
     *         otherwise
     */
    BRAYNS_API bool empty() const;

    /** @return true if the geometry Model is dirty, false otherwise */
    BRAYNS_API bool dirty() const;

    /**
        Returns the bounds for the Model
    */
    const Boxf& getBounds() const { return _bounds; }
    template <typename T>
    void updateBounds(const T& value)
    {
        _bounds.merge(value);
    }
    void setBounds(const Boxf& box) { _bounds = box; }
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
        Returns streamlines handled by the model
      */
    BRAYNS_API StreamlinesMap& getStreamlines() { return _streamlines; }
    /**
      Adds a streamline to the model
      @param materialId Id of the material for the streamline
      @param vertices Vertices of the streamline
      @param colors Colors of the vertices (RGBA)
      @param radii Radius of the vertices (optional)
      */
    BRAYNS_API void addStreamline(const size_t materialId,
                                  const Vector3fs& vertices,
                                  const Vector4fs& colors,
                                  const std::vector<float>& radii = {});

    /**
      Adds a SDFGeometry to the scene
      @param materialId Material of the geometry
      @param geom Geometry to add
      @param neighbourIndices Global indices of the geometries to smoothly blend
      together with
      @return Global index of the geometry
      */
    uint64_t addSDFGeometry(const size_t materialId, const SDFGeometry& geom,
                            const std::vector<size_t>& neighbourIndices);

    /** Update the list of neighbours for a SDF geometry
      @param geometryIdx Index of the geometry
      @param neighbourIndices Global indices of the geometries to smoothly blend
      together with
      */
    void updateSDFGeometryNeighbours(
        size_t geometryIdx, const std::vector<size_t>& neighbourIndices);

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
    BRAYNS_API void createMissingMaterials(
        const bool castSimulationData = false);

    /**
     * @brief getMaterials Returns a reference to the map of materials handled
     * by the model
     * @return The map of materials handled by the model
     */
    BRAYNS_API const MaterialMap& getMaterials() const { return _materials; }
    /**
     * @brief getMaterial Returns a pointer to a specific material
     * @param materialId Id of the material
     * @return A pointer to the material or an exception if the material is not
     * registered in the model
     */
    BRAYNS_API MaterialPtr getMaterial(const size_t materialId) const;

    BRAYNS_API virtual void buildBoundingBox() = 0;

    /** @return the size in bytes of all geometries. */
    size_t getSizeInBytes() const { return _sizeInBytes; }
    void markInstancesDirty() { _instancesDirty = true; }
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
    StreamlinesMap _streamlines;
    bool _streamlinesDirty{true};
    Boxf _bounds;
    bool _useSimulationModel{false};

    struct SDFGeometryData
    {
        std::vector<SDFGeometry> geometries;
        std::map<size_t, std::vector<uint32_t>> geometryIndices;

        std::vector<std::vector<size_t>> neighbours;
        std::vector<size_t> neighboursFlat;
    };

    SDFGeometryData _sdf;
    bool _sdfGeometriesDirty{false};
    bool _instancesDirty{true};

    size_t _sizeInBytes{0};

    SERIALIZATION_FRIEND(Model)
};
}
#endif // Model_H
