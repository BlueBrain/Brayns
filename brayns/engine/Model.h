/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/BaseObject.h>
#include <brayns/common/MaterialsColorMap.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/VolumeDataType.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/Streamline.h>
#include <brayns/common/geometry/TriangleMesh.h>
#include <brayns/common/propertymap/PropertyMap.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <brayns/engine/BrickedVolume.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/SharedDataVolume.h>

#include <brayns/json/JsonType.h>

#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/VolumeParameters.h>

#include <set>

namespace brayns
{
struct SDFGeometryData
{
    std::vector<SDFGeometry> geometries;
    std::map<size_t, std::vector<uint64_t>> geometryIndices;

    std::vector<std::vector<uint64_t>> neighbours;
    std::vector<uint64_t> neighboursFlat;
};

class Scene;

class ModelInstance : public BaseObject
{
public:
    ModelInstance() = default;
    ModelInstance(const bool visible, const bool boundingBox, const Transformation &transformation);

    bool getVisible() const;
    void setVisible(const bool visible);
    bool getBoundingBox() const;
    void setBoundingBox(const bool enabled);
    const Transformation &getTransformation() const;
    void setTransformation(const Transformation &transformation);
    void setModelID(const size_t id);
    size_t getModelID() const;
    void setInstanceID(const size_t id);
    size_t getInstanceID() const;

protected:
    size_t _modelID{0};
    size_t _instanceID{0};
    bool _visible{true};
    bool _boundingBox{false};
    Transformation _transformation;
};

class ModelParams : public ModelInstance
{
public:
    ModelParams() = default;

    ModelParams(const std::string &path);
    ModelParams(const std::string &name, const std::string &path);
    ModelParams(const std::string &name, const std::string &path, const JsonValue &loaderProperties);

    ModelParams(ModelParams &&rhs) = default;
    ModelParams &operator=(ModelParams &&rhs) = default;

    ModelParams(const ModelParams &rhs) = default;
    ModelParams &operator=(const ModelParams &rhs) = default;

    void setName(const std::string &name);
    const std::string &getName() const;
    void setPath(const std::string &path);
    const std::string &getPath() const;
    void setLoaderName(const std::string &loaderName);
    const std::string &getLoaderName() const;
    const JsonValue &getLoadParameters() const;
    void setLoadParameters(const JsonValue &pm);

protected:
    std::string _name;
    std::string _path;
    std::string _loaderName;
    JsonValue _loadParameters;
};

/**
 * @brief The BinaryParam struct extends the ModelParams to allow adding models
 * from blobs of memory
 */
struct BinaryParam : ModelParams
{
    size_t size{0}; //!< size in bytes of file
    std::string type; //!< file extension or type (MESH, POINTS, CIRCUIT)
    std::string chunksID;
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

class Model;
using ModelPtr = std::unique_ptr<Model>;
using ModelMetadata = std::map<std::string, std::string>;

class ModelDescriptor : public ModelParams
{
public:
    using RemovedCallback = std::function<void(const ModelDescriptor &)>;

    ModelDescriptor() = default;

    ModelDescriptor(ModelDescriptor &&rhs) = default;
    ModelDescriptor &operator=(ModelDescriptor &&rhs) = default;

    ModelDescriptor(ModelPtr model, const std::string &path);
    ModelDescriptor(ModelPtr model, const std::string &path, const ModelMetadata &metadata);
    ModelDescriptor(ModelPtr model, const std::string &name, const std::string &path, const ModelMetadata &metadata);

    ModelDescriptor &operator=(const ModelParams &rhs);

    bool getEnabled() const;

    void setMetadata(const ModelMetadata &metadata);

    const ModelMetadata &getMetadata() const;

    const Model &getModel() const;

    Model &getModel();

    void addInstance(const ModelInstance &instance);

    void removeInstance(const size_t id);

    ModelInstance *getInstance(const size_t id);

    const std::vector<ModelInstance> &getInstances() const;

    Boxd getBounds() const;

    void computeBounds();

    void setProperties(const PropertyMap &properties);

    const PropertyMap &getProperties() const;

    /**
     * Set a function that is called when this model is about to be removed.
     */
    void addOnRemoved(const RemovedCallback &callback);

    /** @internal */
    void callOnRemoved();

    /** @internal */
    void markForRemoval();

    /** @internal */
    bool isMarkedForRemoval() const;

    /** @internal */
    std::shared_ptr<ModelDescriptor> clone(ModelPtr model) const;

private:
    size_t _nextInstanceID{0};
    Boxd _bounds;
    ModelMetadata _metadata;
    ModelPtr _model;
    std::vector<ModelInstance> _instances;
    PropertyMap _properties;
    std::vector<RemovedCallback> _onRemovedCallback;
    bool _markedForRemoval = false;
};

using ModelDescriptorPtr = std::shared_ptr<ModelDescriptor>;

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
    Model(AnimationParameters &animationParameters, VolumeParameters &volumeParameters);

    virtual ~Model();

    /** @name API for engine-specific code */
    //@{
    virtual void commitGeometry() = 0;

    /** Factory method to create an engine-specific material. */
    MaterialPtr createMaterial(const size_t materialId, const std::string &name, const PropertyMap &properties = {});

    /**
     * Create a volume with the given dimensions, voxel spacing and data type
     * where the voxels are set via setVoxels() from any memory location.
     */
    virtual SharedDataVolumePtr createSharedDataVolume(
        const Vector3ui &dimensions,
        const Vector3f &spacing,
        const VolumeDataType type) const = 0;

    /**
     * Create a volume with the given dimensions, voxel spacing and data type
     * where the voxels are copied via setBrick() into an optimized internal
     * storage.
     */
    virtual BrickedVolumePtr
        createBrickedVolume(const Vector3ui &dimensions, const Vector3f &spacing, const VolumeDataType type) const = 0;

    virtual void buildBoundingBox() = 0;
    //@}

    /**
     * @return true if the geometry Model does not contain any geometry, false
     *         otherwise
     */
    bool empty() const;

    /** @return true if the geometry Model is dirty, false otherwise */
    bool isDirty() const;

    /**
        Returns the bounds for the Model
    */
    const Boxd &getBounds() const;

    /**
        Returns spheres handled by the Model
    */
    const std::map<size_t, std::vector<Sphere>> &getSpheres() const;

    std::map<size_t, std::vector<Sphere>> &getSpheres();

    /**
      Adds a sphere to the model
      @param materialId Id of the material for the sphere
      @param sphere Sphere to add
      @return Index of the sphere for the specified material
      */
    uint64_t addSphere(const size_t materialId, const Sphere &sphere);

    /**
        Returns cylinders handled by the model
    */
    const std::map<size_t, std::vector<Cylinder>> &getCylinders() const;

    std::map<size_t, std::vector<Cylinder>> &getCylinders();

    /**
      Adds a cylinder to the model
      @param materialId Id of the material for the cylinder
      @param cylinder Cylinder to add
      @return Index of the sphere for the specified material
      */
    uint64_t addCylinder(const size_t materialId, const Cylinder &cylinder);

    /**
        Returns cones handled by the model
    */
    const std::map<size_t, std::vector<Cone>> &getCones() const;

    std::map<size_t, std::vector<Cone>> &getCones();

    /**
      Adds a cone to the model
      @param materialId Id of the material for the cone
      @param cone Cone to add
      @return Index of the sphere for the specified material
      */
    uint64_t addCone(const size_t materialId, const Cone &cone);

    /**
      Adds a streamline to the model
      @param materialId Id of the material for the streamline
      @param streamline Streamline to add
      */
    void addStreamline(const size_t materialId, const Streamline &streamline);

    /**
        Returns streamlines handled by the model
    */
    std::map<size_t, StreamlinesData> &getStreamlines();

    /**
      Adds a SDFGeometry to the scene
      @param materialId Material of the geometry
      @param geom Geometry to add
      @param neighbourIndices Global indices of the geometries to smoothly blend
      together with
      @return Global index of the geometry
      */
    uint64_t
        addSDFGeometry(const size_t materialId, const SDFGeometry &geom, const std::vector<size_t> &neighbourIndices);

    /**
     * Returns SDF geometry data handled by the model
     */
    SDFGeometryData &getSDFGeometryData();

    /** Update the list of neighbours for a SDF geometry
      @param geometryIdx Index of the geometry
      @param neighbourIndices Global indices of the geometries to smoothly blend
      together with
      */
    void updateSDFGeometryNeighbours(size_t geometryIdx, const std::vector<size_t> &neighbourIndices);

    /**
        Returns triangle meshes handled by the model
    */
    const std::map<size_t, TriangleMesh> &getTriangleMeshes() const;

    std::map<size_t, TriangleMesh> &getTriangleMeshes();

    /** Add a volume to the model*/
    void addVolume(VolumePtr);

    /** Remove a volume from the model */
    void removeVolume(VolumePtr);

    /**
     * @brief logInformation Logs information about the model, like the number
     * of primitives, and the associated memory footprint.
     */
    void logInformation();

    /**
        Sets the materials handled by the model, and available to the geometry
        @param colorMap Specifies the algorithm that is used to create the
       materials. For instance MT_RANDOM creates materials with random colors,
       transparency, reflection, and light emission
    */
    void setMaterialsColorMap(const MaterialsColorMap colorMap);

    /**
     * @brief getMaterials Returns a reference to the map of materials handled
     * by the model
     * @return The map of materials handled by the model
     */
    const std::map<size_t, MaterialPtr> &getMaterials() const;

    /**
     * @brief getMaterial Returns a pointer to a specific material
     * @param materialId Id of the material
     * @return A pointer to the material or an exception if the material is not
     * registered in the model
     */
    MaterialPtr getMaterial(const size_t materialId) const;

    /**
        Returns the simulutation handler
    */
    AbstractSimulationHandlerPtr getSimulationHandler() const;

    /**
        Sets the simulation handler
    */
    void setSimulationHandler(AbstractSimulationHandlerPtr handler);

    /** @return the size in bytes of all geometries. */
    size_t getSizeInBytes() const;

    void markInstancesDirty();

    void markInstancesClean();

    const std::vector<VolumePtr> &getVolumes() const;

    bool isVolumesDirty() const;

    void resetVolumesDirty();

    void setSimulationEnabled(const bool v);

    bool isSimulationEnabled() const;

    void updateBounds();

    /** @internal */
    void copyFrom(const Model &rhs);

protected:
    void _updateSizeInBytes();

    /** Factory method to create an engine-specific material. */
    virtual MaterialPtr createMaterialImpl(const PropertyMap &properties = {}) = 0;

    /** Mark all geometries as clean. */
    void _markGeometriesClean();

    AnimationParameters &_animationParameters;
    VolumeParameters &_volumeParameters;

    AbstractSimulationHandlerPtr _simulationHandler;
    bool _simulationEnabledDirty{true};
    bool _simulationEnabled{false};

    std::map<size_t, MaterialPtr> _materials;

    struct Geometries
    {
        std::map<size_t, std::vector<Sphere>> _spheres;
        std::map<size_t, std::vector<Cylinder>> _cylinders;
        std::map<size_t, std::vector<Cone>> _cones;
        std::map<size_t, TriangleMesh> _triangleMeshes;
        std::map<size_t, StreamlinesData> _streamlines;
        SDFGeometryData _sdf;
        std::vector<VolumePtr> _volumes;

        Boxd _sphereBounds;
        Boxd _cylindersBounds;
        Boxd _conesBounds;
        Boxd _triangleMeshesBounds;
        Boxd _streamlinesBounds;
        Boxd _sdfGeometriesBounds;
        Boxd _volumesBounds;

        bool isEmpty() const;
    };

    // the model clone actually shares all geometries to save memory. It will
    // still create engine specific copies though (BVH only ideally) as part of
    // commitGeometry()
    std::shared_ptr<Geometries> _geometries{std::make_shared<Geometries>()};

    bool _spheresDirty{false};
    bool _cylindersDirty{false};
    bool _conesDirty{false};
    bool _triangleMeshesDirty{false};
    bool _streamlinesDirty{false};
    bool _sdfGeometriesDirty{false};
    bool _volumesDirty{false};

    bool _areGeometriesDirty() const;

    Boxd _bounds;
    bool _instancesDirty{true};
    size_t _sizeInBytes{0};

    // Whether this model has set the AnimationParameters "is ready" callback
    bool _isReadyCallbackSet{false};
};
} // namespace brayns
