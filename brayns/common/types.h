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

#ifndef TYPES_H
#define TYPES_H

#define SERIALIZATION_ACCESS(type)            \
    namespace staticjson                      \
    {                                         \
    class ObjectHandler;                      \
    void init(brayns::type*, ObjectHandler*); \
    }
#define SERIALIZATION_FRIEND(type) \
    friend void staticjson::init(type*, staticjson::ObjectHandler*);

#include <brayns/common/mathTypes.h>

#include <array>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <vector>

typedef ::int64_t int64;
typedef ::uint64_t uint64;
typedef ::int32_t int32;
typedef ::uint32_t uint32;
typedef ::int16_t int16;
typedef ::uint16_t uint16;
typedef ::int8_t int8;
typedef ::uint8_t uint8;
typedef ::int64_t index_t;

typedef std::vector<std::string> strings;
typedef std::vector<float> floats;
typedef std::vector<int> ints;
typedef std::vector<unsigned int> uints;
typedef std::vector<int8_t> int8_ts;
typedef std::vector<uint8_t> uint8_ts;
typedef std::vector<int16_t> int16_ts;
typedef std::vector<uint16_t> uint16_ts;
typedef std::vector<int32_t> int32_ts;
typedef std::vector<uint32_t> uint32_ts;
typedef std::vector<int64_t> int64_ts;
typedef std::vector<uint64_t> uint64_ts;
typedef std::vector<size_t> size_ts;

#ifdef __GNUC__
#define BRAYNS_UNUSED __attribute__((unused))
#else
#define BRAYNS_UNUSED
#endif

#define BRAYNS_UNUSED_VAR(x) (void)x;

namespace brayns
{
// Forward declarations
class Brayns;

class ActionInterface;

class Engine;
using EnginePtr = std::shared_ptr<Engine>;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

class AbstractManipulator;
using AbstractManipulatorPtr = std::shared_ptr<AbstractManipulator>;

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

class TransferFunction;
class Renderer;
using RendererPtr = std::shared_ptr<Renderer>;

class FrameBuffer;
using FrameBufferPtr = std::shared_ptr<FrameBuffer>;

class Model;
using ModelPtr = std::unique_ptr<Model>;
using ModelMetadata = std::map<std::string, std::string>;

class Transformation;
using Transformations = std::vector<Transformation>;

class ModelInstance;
class ModelParams;
class ModelDescriptor;
using ModelDescriptorPtr = std::shared_ptr<ModelDescriptor>;
using ModelDescriptors = std::vector<ModelDescriptorPtr>;
using ModelInstances = std::vector<ModelInstance>;

class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialMap = std::map<size_t, MaterialPtr>;

class ClipPlane;
using ClipPlanePtr = std::shared_ptr<ClipPlane>;
using ClipPlanes = std::vector<ClipPlanePtr>;

struct Sphere;
using Spheres = std::vector<Sphere>;
using SpheresMap = std::map<size_t, Spheres>;

struct Cylinder;
using Cylinders = std::vector<Cylinder>;
using CylindersMap = std::map<size_t, Cylinders>;

struct Cone;
using Cones = std::vector<Cone>;
using ConesMap = std::map<size_t, Cones>;

struct TrianglesMesh;
using TrianglesMeshMap = std::map<size_t, TrianglesMesh>;

struct StreamlinesData;
using StreamlinesDataMap = std::map<size_t, StreamlinesData>;

class Volume;
class BrickedVolume;
class SharedDataVolume;
using VolumePtr = std::shared_ptr<Volume>;
using SharedDataVolumePtr = std::shared_ptr<SharedDataVolume>;
using BrickedVolumePtr = std::shared_ptr<BrickedVolume>;
using Volumes = std::vector<VolumePtr>;

class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
using TexturesMap = std::map<std::string, Texture2DPtr>;

class Light;
using LightPtr = std::shared_ptr<Light>;
using Lights = std::vector<LightPtr>;

class DirectionalLight;
using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;

class PointLight;

class AbstractSimulationHandler;
using AbstractSimulationHandlerPtr = std::shared_ptr<AbstractSimulationHandler>;

class CircuitSimulationHandler;
using CircuitSimulationHandlerPtr = std::shared_ptr<CircuitSimulationHandler>;

class SpikeSimulationHandler;
using SpikeSimulationHandlerPtr = std::shared_ptr<SpikeSimulationHandler>;

class CADiffusionSimulationHandler;
using CADiffusionSimulationHandlerPtr =
    std::shared_ptr<CADiffusionSimulationHandler>;

class AbstractParameters;
class AnimationParameters;
class ApplicationParameters;
class GeometryParameters;
class ParametersManager;
class RenderingParameters;
class SceneParameters;
class StreamParameters;
class VolumeParameters;

class PluginAPI;
class ExtensionPlugin;
using ExtensionPluginPtr = std::shared_ptr<ExtensionPlugin>;
using ExtensionPlugins = std::vector<ExtensionPluginPtr>;

class KeyboardHandler;

class MeshLoader;

class Statistics;

enum class EngineType
{
    ospray,
    optix
};

/** Define the frame buffer format */
enum class FrameBufferFormat
{
    rgba_i8,
    bgra_i8,
    rgb_i8,
    rgb_f32,
    none
};

/** Define the color scheme to be applied to the geometry */
enum class ColorScheme
{
    none = 0,
    neuron_by_id = 1,
    neuron_by_type = 2,
    neuron_by_segment_type = 3,
    neuron_by_layer = 4,
    neuron_by_mtype = 5,
    neuron_by_etype = 6,
    neuron_by_target = 7,
    protein_by_id = 8,
    protein_atoms = 9,
    protein_chains = 10,
    protein_residues = 11
};

/** Geometry quality */
enum class GeometryQuality
{
    low,
    medium,
    high
};

/** Morphology element types */
enum class MorphologySectionType
{
    soma = 0x01,
    axon = 0x02,
    dendrite = 0x04,
    apical_dendrite = 0x08,
    all = 0xff
};
using MorphologySectionTypes = std::vector<MorphologySectionType>;

template <typename T>
size_t enumsToBitmask(const std::vector<T> enums)
{
    size_t bit{0};
    for (auto& val : enums)
        bit |= size_t(val);
    return bit;
}

/**
 * @brief The Histogram struct contains the range as well as the values of the
 *        simulation histogram for a given simulation frame, determined by the
 *        frame
 */
struct Histogram
{
    uint64_ts values;
    Vector2d range;
    uint32_t frame;
    bool empty() const { return values.empty(); }
};

/** Some 'special' materials are used by Brayns to accomplish specific features
 *  such as bounding boxes.
 */
const size_t NO_MATERIAL = std::numeric_limits<size_t>::max();
const size_t BOUNDINGBOX_MATERIAL_ID = NO_MATERIAL - 1;

enum class MemoryMode
{
    shared,
    replicated
};

enum class MaterialsColorMap
{
    none,           // Random colors
    random,         // Random materials including transparency, reflection,
                    // and light emission
    shades_of_grey, // 255 shades of grey
    gradient,       // Gradient from red to yellow
    pastel          // Random pastel colors
};

/**
 * The different modes for moving the camera.
 */
enum class CameraMode
{
    flying,
    inspect
};

/** A clip plane is defined by a normal and a distance expressed
 * in absolute value of the coordinate system. Values are stored
 * in a Vector4, with the following order: nx, ny, nz and d
 */
using Plane = std::array<double, 4>;
using Planes = std::vector<Plane>;

struct RenderInput
{
    Vector2i windowSize;
    Matrix4f modelview;
    Matrix4f projection;

    Vector3d position;
    Vector3d target;
    Vector3d up;
};

struct RenderOutput
{
    Vector2i frameSize;
    uint8_ts colorBuffer;
    floats depthBuffer;
    FrameBufferFormat colorBufferFormat;
};

class Progress;

class AbstractTask;
using TaskPtr = std::shared_ptr<AbstractTask>;

struct Blob
{
    std::string type;
    std::string name;
    std::string data;
};

class Loader;
using LoaderPtr = std::unique_ptr<Loader>;

enum class DataType
{
    FLOAT,
    DOUBLE,
    UINT8,
    UINT16,
    UINT32,
    INT8,
    INT16,
    INT32
};

class PropertyMap;
class PropertyObject;

/** Description for RPC with no parameter. */
struct RpcDescription
{
    std::string methodName;
    std::string methodDescription;
};

/** Description for RPC with one parameter. */
struct RpcParameterDescription
{
    std::string methodName;
    std::string methodDescription;
    std::string paramName;
    std::string paramDescription;
};
}

#endif // TYPES_H
