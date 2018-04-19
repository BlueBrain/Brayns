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

#include <boost/program_options.hpp>

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

#if BRAYNS_USE_MAGICKPP
#define MAGICKCORE_HDRI_ENABLE true
#define MAGICKCORE_QUANTUM_DEPTH 32
#endif

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
typedef std::shared_ptr<Engine> EnginePtr;

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class AbstractManipulator;
typedef std::shared_ptr<AbstractManipulator> AbstractManipulatorPtr;

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class TransferFunction;
class Renderer;
typedef std::shared_ptr<Renderer> RendererPtr;
typedef std::vector<RendererPtr> Renderers;

class FrameBuffer;
typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

struct Sphere;
typedef std::vector<Sphere> Spheres;
typedef std::map<size_t, Spheres> SpheresMap;

struct Cylinder;
typedef std::vector<Cylinder> Cylinders;
typedef std::map<size_t, Cylinders> CylindersMap;

struct Cone;
typedef std::vector<Cone> Cones;
typedef std::map<size_t, Cones> ConesMap;

struct TrianglesMesh;
typedef std::map<size_t, TrianglesMesh> TrianglesMeshMap;

class Material;
typedef std::vector<Material> Materials;

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;
typedef std::map<std::string, Texture2DPtr> TexturesMap;

class Light;
typedef std::shared_ptr<Light> LightPtr;
typedef std::vector<LightPtr> Lights;

class DirectionalLight;
typedef std::shared_ptr<DirectionalLight> DirectionalLightPtr;

class PointLight;

class AbstractSimulationHandler;
typedef std::shared_ptr<AbstractSimulationHandler> AbstractSimulationHandlerPtr;

class CircuitSimulationHandler;
typedef std::shared_ptr<CircuitSimulationHandler> CircuitSimulationHandlerPtr;

class SpikeSimulationHandler;
typedef std::shared_ptr<SpikeSimulationHandler> SpikeSimulationHandlerPtr;

class CADiffusionSimulationHandler;
typedef std::shared_ptr<CADiffusionSimulationHandler>
    CADiffusionSimulationHandlerPtr;

class VolumeHandler;
typedef std::shared_ptr<VolumeHandler> VolumeHandlerPtr;

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
typedef std::shared_ptr<ExtensionPlugin> ExtensionPluginPtr;
typedef std::vector<ExtensionPluginPtr> ExtensionPlugins;

class ExtensionPluginFactory;

class RocketsPlugin;

class DeflectPlugin;

class KeyboardHandler;

class MeshLoader;

class Statistics;

/** Types of renderers */
enum class RendererType
{
    default_,
    proximity,
    simulation,
    particle,
    geometryNormals,
    shadingNormals,
    scientificvisualization
};
typedef std::vector<RendererType> RendererTypes;
typedef std::map<RendererType, RendererPtr> RendererMap;

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

/** Define the environment that is added to the default scene */
enum class SceneEnvironment
{
    none,
    ground,
    wall,
    bounding_box
};

/** Define light types */
enum class LightType
{
    point = 0,
    directional,
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
    Vector2f range;
    uint32_t frame;
    bool empty() const { return values.empty(); }
};

/** Some 'special' materials are used by Brayns to accomplish specific features
 *  such as skyboxes.
 */
const size_t NO_MATERIAL = std::numeric_limits<size_t>::max();
const size_t NB_SYSTEM_MATERIALS = 6;
const size_t MATERIAL_SYSTEM = 0;
const std::string TEXTURE_NAME_SKYBOX = "SKYBOX";
const std::string TEXTURE_NAME_SIMULATION = "SIMULATION";

enum class MemoryMode
{
    shared,
    replicated
};

/** Defines how materials should be created */
enum class MaterialType
{
    surface = 0,            // Material for surfaces (default)
    skybox = 1,             // Material for the skybox
    bounding_box = 2,       // Material for bounding boxes
    voltage_simulation = 3, // Material for voltage simulation data
    calcium_simulation = 4, // Material for calcium simulation data
    invisible = 5           // Material used to hide geometry
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

/** Generic action that can be performed on a scene object */
enum class Action
{
    create,
    update
};

enum class ShadingType
{
    none,
    diffuse,
    electron,
};

enum class CameraType
{
    default_, // Perspective
    orthographic,
    panoramic,
    clipped
};

enum class StereoMode
{
    none = 0,
    left,
    right,
    side_by_side
};

enum class IndexMode
{
    unchanged,
    modulo,
    bounded
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
using ClipPlane = Vector4f;
using ClipPlanes = std::array<ClipPlane, 6>;

struct RenderInput
{
    Vector2i windowSize;
    Matrix4f modelview;
    Matrix4f projection;

    Vector3f position;
    Vector3f target;
    Vector3f up;
};

struct RenderOutput
{
    uint8_ts colorBuffer;
    floats depthBuffer;
    FrameBufferFormat colorBufferFormat;
};

class Progress;

class Task;
using TaskPtr = std::shared_ptr<Task>;

struct Blob
{
    std::string type;
    std::string name;
    std::string data;
};
}

#endif // TYPES_H
