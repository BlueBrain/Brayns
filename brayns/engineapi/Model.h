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

#pragma once

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/PropertyMap.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFBezier.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/Streamline.h>
#include <brayns/common/geometry/TriangleMesh.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/common/types.h>

#include <set>

SERIALIZATION_ACCESS(Model)
SERIALIZATION_ACCESS(ModelParams)
SERIALIZATION_ACCESS(ModelDescriptor)
SERIALIZATION_ACCESS(ModelInstance)

namespace brayns
{
struct SDFGeometryData
{
    std::vector<SDFGeometry> geometries;
    std::map<size_t, std::vector<uint64_t>> geometryIndices;

    std::vector<std::vector<uint64_t>> neighbours;
    std::vector<uint64_t> neighboursFlat;
};

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
    ModelParams(const std::string& name, const std::string& path,
                const PropertyMap& loaderProperties);

    ModelParams(ModelParams&& rhs) = default;
    ModelParams& operator=(ModelParams&& rhs) = default;

    ModelParams(const ModelParams& rhs) = default;
    ModelParams& operator=(const ModelParams& rhs) = default;

    void setName(const std::string& name) { _updateValue(_name, name); }
    const std::string& getName() const { return _name; }
    void setPath(const std::string& path) { _updateValue(_path, path); }
    const std::string& getPath() const { return _path; }
    void setLoaderName(const std::string& loaderName)
    {
        _updateValue(_loaderName, loaderName);
    }
    const std::string& getLoaderName() const { return _loaderName; }
    const PropertyMap& getLoaderProperties() const { return _loaderProperties; }
    void setLoaderProperties(const PropertyMap& pm) { _loaderProperties = pm; }

protected:
    std::string _name;
    std::string _path;
    std::string _loaderName;
    PropertyMap _loaderProperties;

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
    void setMetadata(const ModelMetadata& metadata)
    {
        _metadata = metadata;
        markModified();
    }
    const ModelMetadata& getMetadata() const { return _metadata; }
    const Model& getModel() const { return *_model; }
    Model& getModel() { return *_model; }
    void addInstance(const ModelInstance& instance);
    void removeInstance(const size_t id);
    ModelInstance* getInstance(const size_t id);
    const ModelInstances& getInstances() const { return _instances; }
    Boxd getBounds() const { return _bounds; }
    void computeBounds();

    void setProperties(const PropertyMap& properties)
    {
        _properties = properties;
        markModified();
    }

    const PropertyMap& getProperties() const { return _properties; }
    using RemovedCallback = std::function<void(const ModelDescriptor&)>;

    /**
     * Set a function that is called when this model is about to be removed.
     */
    void onRemoved(const RemovedCallback& callback)
    {
        _onRemovedCallback = callback;
    }

    /** @internal */
    void callOnRemoved()
    {
        if (_onRemovedCallback)
            _onRemovedCallback(*this);
    }
    /** @internal */
    void markForRemoval() { _markedForRemoval = true; }
    /** @internal */
    bool isMarkedForRemoval() const { return _markedForRemoval; }
    /** @internal */
    ModelDescriptorPtr clone(ModelPtr model) const;

private:
    size_t _nextInstanceID{0};
    Boxd _bounds;
    ModelMetadata _metadata;
    ModelPtr _model;
    ModelInstances _instances;
    PropertyMap _properties;
    RemovedCallback _onRemovedCallback;
    bool _markedForRemoval = false;

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
    Model(AnimationParameters& animationParameters,
          VolumeParameters& volumeParameters);

    BRAYNS_API virtual ~Model();

    /** @name API for engine-specific code */
    //@{
    virtual void commitGeometry() = 0;

    /** Commit transfer function */
    bool commitTransferFunction();

    /** Commit simulation data */
    bool commitSimulationData();

    /** Factory method to create an engine-specific material. */
    BRAYNS_API MaterialPtr createMaterial(const size_t materialId,
                                          const std::string& name,
                                          const PropertyMap& properties = {});

    /**
     * Create a volume with the given dimensions, voxel spacing and data type
     * where the voxels are set via setVoxels() from any memory location.
     */
    BRAYNS_API virtual SharedDataVolumePtr createSharedDataVolume(
        const Vector3ui& dimensions, const Vector3f& spacing,
        const DataType type) const = 0;

    /**
     * Create a volume with the given dimensions, voxel spacing and data type
     * where the voxels are copied via setBrick() into an optimized internal
     * storage.
     */
    BRAYNS_API virtual BrickedVolumePtr createBrickedVolume(
        const Vector3ui& dimensions, const Vector3f& spacing,
        const DataType type) const = 0;

    BRAYNS_API virtual void buildBoundingBox() = 0;
    //@}

    /**
     * @return true if the geometry Model does not contain any geometry, false
     *         otherwise
     */
    BRAYNS_API bool empty() const;

    /** @return true if the geometry Model is dirty, false otherwise */
    BRAYNS_API bool isDirty() const;

    /**
        Returns the bounds for the Model
    */
    const Boxd& getBounds() const { return _bounds; }
    /**
        Returns spheres handled by the Model
    */
    const SpheresMap& getSpheres() const { return _geometries->_spheres; }
    SpheresMap& getSpheres()
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
    BRAYNS_API uint64_t addSphere(const size_t materialId,
                                  const Sphere& sphere);

    /**
        Returns cylinders handled by the model
      */
    const CylindersMap& getCylinders() const { return _geometries->_cylinders; }
    CylindersMap& getCylinders()
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
    BRAYNS_API uint64_t addCylinder(const size_t materialId,
                                    const Cylinder& cylinder);
    /**
        Returns cones handled by the model
    */
    const ConesMap& getCones() const { return _geometries->_cones; }
    ConesMap& getCones()
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
    BRAYNS_API uint64_t addCone(const size_t materialId, const Cone& cone);

    /**
        Returns SDFBezier handled by the model
    */
    const SDFBeziersMap& getSDFBeziers() const
    {
        return _geometries->_sdfBeziers;
    }

    SDFBeziersMap& getSDFBeziers()
    {
        _sdfBeziersDirty = true;
        return _geometries->_sdfBeziers;
    }
    /**
      Adds a SDFBezier to the model
      @param materialId Id of the material for the sdfBezier
      @param sdfBezier SDFBezier to add
      @return Index of the bezier for the specified material
      */
    BRAYNS_API uint64_t addSDFBezier(const size_t materialId,
                                     const SDFBezier& sdfBezier);

    /**
      Adds a streamline to the model
      @param materialId Id of the material for the streamline
      @param streamline Streamline to add
      */
    BRAYNS_API void addStreamline(const size_t materialId,
                                  const Streamline& streamline);

    /**
        Returns streamlines handled by the model
    */
    const StreamlinesDataMap& getStreamlines() const
    {
        return _geometries->_streamlines;
    }
    StreamlinesDataMap& getStreamlines()
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
    uint64_t addSDFGeometry(const size_t materialId, const SDFGeometry& geom,
                            const uint64_ts& neighbourIndices);

    /**
     * Returns SDF geometry data handled by the model
     */
    const SDFGeometryData& getSDFGeometryData() const
    {
        return _geometries->_sdf;
    }
    SDFGeometryData& getSDFGeometryData()
    {
        _sdfGeometriesDirty = true;
        return _geometries->_sdf;
    }

    /** Update the list of neighbours for a SDF geometry
      @param geometryIdx Index of the geometry
      @param neighbourIndices Global indices of the geometries to smoothly blend
      together with
      */
    void updateSDFGeometryNeighbours(size_t geometryIdx,
                                     const uint64_ts& neighbourIndices);

    /**
        Returns triangle meshes handled by the model
    */
    const TriangleMeshMap& getTriangleMeshes() const
    {
        return _geometries->_triangleMeshes;
    }
    TriangleMeshMap& getTriangleMeshes()
    {
        _triangleMeshesDirty = true;
        return _geometries->_triangleMeshes;
    }

    /** Add a volume to the model*/
    BRAYNS_API void addVolume(VolumePtr);

    /** Remove a volume from the model */
    BRAYNS_API void removeVolume(VolumePtr);

    /**
     * @brief logInformation Logs information about the model, like the number
     * of primitives, and the associated memory footprint.
     */
    BRAYNS_API void logInformation();

    /**
        Sets the materials handled by the model, and available to the geometry
        @param colorMap Specifies the algorithm that is used to create the
       materials. For instance MT_RANDOM creates materials with random colors,
       transparency, reflection, and light emission
    */
    void BRAYNS_API setMaterialsColorMap(const MaterialsColorMap colorMap);

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

    /** @return the transfer function used for volumes and simulations. */
    TransferFunction& getTransferFunction() { return _transferFunction; }
    /** @return the transfer function used for volumes and simulations. */
    const TransferFunction& getTransferFunction() const
    {
        return _transferFunction;
    }

    /**
        Returns the simulutation handler
    */
    BRAYNS_API AbstractSimulationHandlerPtr getSimulationHandler() const;

    /**
        Sets the simulation handler
    */
    BRAYNS_API void setSimulationHandler(AbstractSimulationHandlerPtr handler);

    /** @return the size in bytes of all geometries. */
    size_t getSizeInBytes() const;
    void markInstancesDirty() { _instancesDirty = true; }
    void markInstancesClean() { _instancesDirty = false; }
    const Volumes& getVolumes() const { return _geometries->_volumes; }
    bool isVolumesDirty() const { return _volumesDirty; }
    void resetVolumesDirty() { _volumesDirty = false; }
    void setBVHFlags(std::set<BVHFlag> bvhFlags)
    {
        _bvhFlags = std::move(bvhFlags);
    }
    const std::set<BVHFlag>& getBVHFlags() const { return _bvhFlags; }
    void updateBounds();
    /** @internal */
    void copyFrom(const Model& rhs);

protected:
    void _updateSizeInBytes();

    /** Factory method to create an engine-specific material. */
    BRAYNS_API virtual MaterialPtr createMaterialImpl(
        const PropertyMap& properties = {}) = 0;

    /** Mark all geometries as clean. */
    void _markGeometriesClean();

    virtual void _commitTransferFunctionImpl(const Vector3fs& colors,
                                             const floats& opacities,
                                             const Vector2d valueRange) = 0;
    virtual void _commitSimulationDataImpl(const float* frameData,
                                           const size_t frameSize) = 0;

    AnimationParameters& _animationParameters;
    VolumeParameters& _volumeParameters;

    AbstractSimulationHandlerPtr _simulationHandler;
    TransferFunction _transferFunction;

    MaterialMap _materials;

    struct Geometries
    {
        SpheresMap _spheres;
        CylindersMap _cylinders;
        ConesMap _cones;
        SDFBeziersMap _sdfBeziers;
        TriangleMeshMap _triangleMeshes;
        StreamlinesDataMap _streamlines;
        SDFGeometryData _sdf;
        Volumes _volumes;

        Boxd _sphereBounds;
        Boxd _cylindersBounds;
        Boxd _conesBounds;
        Boxd _sdfBeziersBounds;
        Boxd _triangleMeshesBounds;
        Boxd _streamlinesBounds;
        Boxd _sdfGeometriesBounds;
        Boxd _volumesBounds;

        bool isEmpty() const
        {
            return _spheres.empty() && _cylinders.empty() && _cones.empty() &&
                   _sdfBeziers.empty() && _triangleMeshes.empty() &&
                   _sdf.geometries.empty() && _streamlines.empty() &&
                   _volumes.empty();
        }
    };

    // the model clone actually shares all geometries to save memory. It will
    // still create engine specific copies though (BVH only ideally) as part of
    // commitGeometry()
    std::shared_ptr<Geometries> _geometries{std::make_shared<Geometries>()};

    bool _spheresDirty{false};
    bool _cylindersDirty{false};
    bool _conesDirty{false};
    bool _sdfBeziersDirty{false};
    bool _triangleMeshesDirty{false};
    bool _streamlinesDirty{false};
    bool _sdfGeometriesDirty{false};
    bool _volumesDirty{false};

    bool _areGeometriesDirty() const
    {
        return _spheresDirty || _cylindersDirty || _conesDirty ||
               _sdfBeziersDirty || _triangleMeshesDirty || _sdfGeometriesDirty;
    }

    Boxd _bounds;
    bool _instancesDirty{true};
    std::set<BVHFlag> _bvhFlags;
    size_t _sizeInBytes{0};

    // Whether this model has set the AnimationParameters "is ready" callback
    bool _isReadyCallbackSet{false};

    SERIALIZATION_FRIEND(Model)
};
} // namespace brayns
