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
    ModelInstance(const bool visible, const bool boundingBox, const Transformation &transformation)
        : _visible(visible)
        , _boundingBox(boundingBox)
        , _transformation(transformation)
    {
    }
    bool getVisible() const
    {
        return _visible;
    }
    void setVisible(const bool visible)
    {
        _updateValue(_visible, visible);
    }
    bool getBoundingBox() const
    {
        return _boundingBox;
    }
    void setBoundingBox(const bool enabled)
    {
        _updateValue(_boundingBox, enabled);
    }
    const Transformation &getTransformation() const
    {
        return _transformation;
    }
    void setTransformation(const Transformation &transformation)
    {
        _updateValue(_transformation, transformation);
    }

    void setModelID(const size_t id)
    {
        _updateValue(_modelID, id);
    }
    size_t getModelID() const
    {
        return _modelID;
    }
    void setInstanceID(const size_t id)
    {
        _updateValue(_instanceID, id);
    }
    size_t getInstanceID() const
    {
        return _instanceID;
    }

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

    void setName(const std::string &name)
    {
        _updateValue(_name, name);
    }
    const std::string &getName() const
    {
        return _name;
    }
    void setPath(const std::string &path)
    {
        _updateValue(_path, path);
    }
    const std::string &getPath() const
    {
        return _path;
    }
    void setLoaderName(const std::string &loaderName)
    {
        _updateValue(_loaderName, loaderName);
    }
    const std::string &getLoaderName() const
    {
        return _loaderName;
    }
    const JsonValue &getLoadParameters() const
    {
        return _loadParameters;
    }
    void setLoadParameters(const JsonValue &pm)
    {
        _loadParameters = pm;
    }

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
    ModelDescriptor() = default;

    ModelDescriptor(ModelDescriptor &&rhs) = default;
    ModelDescriptor &operator=(ModelDescriptor &&rhs) = default;

    ModelDescriptor(ModelPtr model, const std::string &path);
    ModelDescriptor(ModelPtr model, const std::string &path, const ModelMetadata &metadata);
    ModelDescriptor(ModelPtr model, const std::string &name, const std::string &path, const ModelMetadata &metadata);

    ModelDescriptor &operator=(const ModelParams &rhs);

    bool getEnabled() const
    {
        return _visible || _boundingBox;
    }
    void setMetadata(const ModelMetadata &metadata)
    {
        _metadata = metadata;
        markModified();
    }
    const ModelMetadata &getMetadata() const
    {
        return _metadata;
    }
    const Model &getModel() const
    {
        return *_model;
    }
    Model &getModel()
    {
        return *_model;
    }
    void addInstance(const ModelInstance &instance);
    void removeInstance(const size_t id);
    ModelInstance *getInstance(const size_t id);
    const std::vector<ModelInstance> &getInstances() const
    {
        return _instances;
    }
    Boxd getBounds() const
    {
        return _bounds;
    }
    void computeBounds();

    void setProperties(const PropertyMap &properties)
    {
        _properties = properties;
        markModified();
    }

    const PropertyMap &getProperties() const
    {
        return _properties;
    }
    using RemovedCallback = std::function<void(const ModelDescriptor &)>;

    /**
     * Set a function that is called when this model is about to be removed.
     */
    void addOnRemoved(const RemovedCallback &callback)
    {
        _onRemovedCallback.push_back(callback);
    }

    /** @internal */
    void callOnRemoved()
    {
        if (!_onRemovedCallback.empty())
            for (const auto &callback : _onRemovedCallback)
                callback(*this);
    }
    /** @internal */
    void markForRemoval()
    {
        _markedForRemoval = true;
    }
    /** @internal */
    bool isMarkedForRemoval() const
    {
        return _markedForRemoval;
    }
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
    const Boxd &getBounds() const
    {
        return _bounds;
    }
    /**
        Returns spheres handled by the Model
    */
    const std::map<size_t, std::vector<Sphere>> &getSpheres() const
    {
        return _geometries->_spheres;
    }
    std::map<size_t, std::vector<Sphere>> &getSpheres()
    {
        _spheresDirty = true;
        return _geometries->_spheres;
    }
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
    const std::map<size_t, std::vector<Cylinder>> &getCylinders() const
    {
        return _geometries->_cylinders;
    }
    std::map<size_t, std::vector<Cylinder>> &getCylinders()
    {
        _cylindersDirty = true;
        return _geometries->_cylinders;
    }
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
    const std::map<size_t, std::vector<Cone>> &getCones() const
    {
        return _geometries->_cones;
    }
    std::map<size_t, std::vector<Cone>> &getCones()
    {
        _conesDirty = true;
        return _geometries->_cones;
    }
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
    std::map<size_t, StreamlinesData> &getStreamlines()
    {
        _streamlinesDirty = true;
        return _geometries->_streamlines;
    }
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
    SDFGeometryData &getSDFGeometryData()
    {
        _sdfGeometriesDirty = true;
        return _geometries->_sdf;
    }

    /** Update the list of neighbours for a SDF geometry
      @param geometryIdx Index of the geometry
      @param neighbourIndices Global indices of the geometries to smoothly blend
      together with
      */
    void updateSDFGeometryNeighbours(size_t geometryIdx, const std::vector<size_t> &neighbourIndices);

    /**
        Returns triangle meshes handled by the model
    */
    const std::map<size_t, TriangleMesh> &getTriangleMeshes() const
    {
        return _geometries->_triangleMeshes;
    }
    std::map<size_t, TriangleMesh> &getTriangleMeshes()
    {
        _triangleMeshesDirty = true;
        return _geometries->_triangleMeshes;
    }

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
    const std::map<size_t, MaterialPtr> &getMaterials() const
    {
        return _materials;
    }
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
    void markInstancesDirty()
    {
        _instancesDirty = true;
    }
    void markInstancesClean()
    {
        _instancesDirty = false;
    }
    const std::vector<VolumePtr> &getVolumes() const
    {
        return _geometries->_volumes;
    }
    bool isVolumesDirty() const
    {
        return _volumesDirty;
    }
    void resetVolumesDirty()
    {
        _volumesDirty = false;
    }

    void setSimulationEnabled(const bool v)
    {
        _simulationEnabled = v;
        _simulationEnabledDirty = true;
    }
    bool isSimulationEnabled() const
    {
        return _simulationEnabled;
    }

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

        bool isEmpty() const
        {
            return _spheres.empty() && _cylinders.empty() && _cones.empty() && _triangleMeshes.empty()
                && _sdf.geometries.empty() && _streamlines.empty() && _volumes.empty();
        }
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

    bool _areGeometriesDirty() const
    {
        return _spheresDirty || _cylindersDirty || _conesDirty || _triangleMeshesDirty || _sdfGeometriesDirty;
    }

    Boxd _bounds;
    bool _instancesDirty{true};
    size_t _sizeInBytes{0};

    // Whether this model has set the AnimationParameters "is ready" callback
    bool _isReadyCallbackSet{false};
};
} // namespace brayns
