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
typedef std::shared_ptr<Brayns> BraynsPtr;

class Engine;
typedef std::shared_ptr<Engine> EnginePtr;
typedef std::weak_ptr<Engine> EngineWeakPtr;
typedef std::map<std::string, EnginePtr> EngineMap;

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class AbstractManipulator;
typedef std::shared_ptr<AbstractManipulator> AbstractManipulatorPtr;

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class TransferFunction;
typedef std::shared_ptr<TransferFunction> TransferFunctionPtr;

class Renderer;
typedef std::shared_ptr<Renderer> RendererPtr;
typedef std::vector<RendererPtr> Renderers;

class FrameBuffer;
typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

class ApplicationParameters;
typedef std::shared_ptr<ApplicationParameters> ApplicationParametersPtr;

class GeometryParameters;
typedef std::shared_ptr<GeometryParameters> GeometryParametersPtr;

class RenderingParameters;
typedef std::shared_ptr<RenderingParameters> RenderingParametersPtr;

class VolumeParameters;
typedef std::shared_ptr<VolumeParameters> VolumeParametersPtr;

class ExtensionController;
typedef std::shared_ptr<ExtensionController> ExtensionControllerPtr;

class Geometry;
typedef std::vector<Geometry*> Geometries;

class Primitive;
typedef std::shared_ptr<Primitive> PrimitivePtr;
typedef std::vector<PrimitivePtr> Primitives;
typedef std::map<size_t, Primitives> PrimitivesMap;

class Sphere;
typedef std::shared_ptr<Sphere> SpherePtr;
typedef std::vector<SpherePtr> Spheres;
typedef std::map<size_t, Spheres> SpheresMap;

class Cylinder;
typedef std::shared_ptr<Cylinder> CylinderPtr;
typedef std::vector<CylinderPtr> Cylinders;
typedef std::map<size_t, Cylinders> CylindersMap;

class Cone;
typedef std::shared_ptr<Cone> ConePtr;
typedef std::vector<ConePtr> Cones;
typedef std::map<size_t, Cones> ConesMap;

class TrianglesMesh;
typedef std::map<size_t, TrianglesMesh> TrianglesMeshMap;

class Material;
typedef std::map<size_t, Material> MaterialsMap;

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;
typedef std::map<std::string, Texture2DPtr> TexturesMap;

class Light;
typedef std::shared_ptr<Light> LightPtr;
typedef std::vector<LightPtr> Lights;
typedef std::map<size_t, LightPtr> LightsMap;

class DirectionalLight;
typedef std::shared_ptr<DirectionalLight> DirectionalLightPtr;

class PointLight;
typedef std::shared_ptr<PointLight> PointLightPtr;

class AbstractSimulationHandler;
typedef std::shared_ptr<AbstractSimulationHandler> AbstractSimulationHandlerPtr;

class CircuitSimulationHandler;
typedef std::shared_ptr<CircuitSimulationHandler> CircuitSimulationHandlerPtr;

class Brayns;
class SpikeSimulationHandler;
typedef std::shared_ptr<SpikeSimulationHandler> SpikeSimulationHandlerPtr;

class CADiffusionSimulationHandler;
typedef std::shared_ptr<CADiffusionSimulationHandler>
    CADiffusionSimulationHandlerPtr;

class VolumeHandler;
typedef std::shared_ptr<VolumeHandler> VolumeHandlerPtr;

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

class AbstractParameters;
class ApplicationParameters;
class GeometryParameters;
class RenderingParameters;
class SceneParameters;

class ParametersManager;
typedef std::shared_ptr<ParametersManager> ParametersManagerPtr;

class ExtensionPlugin;
typedef std::shared_ptr<ExtensionPlugin> ExtensionPluginPtr;
typedef std::vector<ExtensionPluginPtr> ExtensionPlugins;

class ExtensionPluginFactory;
typedef std::unique_ptr<ExtensionPluginFactory> ExtensionPluginFactoryPtr;

class RocketsPlugin;
typedef std::shared_ptr<RocketsPlugin> RocketsPluginPtr;

class DeflectPlugin;
typedef std::shared_ptr<DeflectPlugin> DeflectPluginPtr;

class KeyboardHandler;
typedef std::shared_ptr<KeyboardHandler> KeyboardHandlerPtr;

class MeshLoader;

/** Types of renderers */
enum class RendererType
{
    basic,
    proximity,
    simulation,
    particle,
    geometryNormals,
    shadingNormals
};
typedef std::vector<RendererType> RendererTypes;
typedef std::map<RendererType, RendererPtr> RendererMap;

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
    undefined = 0x00,
    soma = 0x01,
    axon = 0x02,
    dendrite = 0x04,
    apical_dendrite = 0x08,
    all = 0xff
};

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

/** Some 'special' materials are used by Brayns to acomplish specific features
 *  such as skyboxes.
 */
const size_t NO_MATERIAL = std::numeric_limits<size_t>::max();
const size_t NB_SYSTEM_MATERIALS = 6;
const size_t MATERIAL_SYSTEM = 0;
const size_t MATERIAL_SKYBOX = 1;
const size_t MATERIAL_BOUNDING_BOX = 2;
const size_t MATERIAL_SIMULATION = 3;
const size_t MATERIAL_INVISIBLE = 4;
const size_t MATERIAL_CA_SIMULATION = 5;
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
    none,           // Random colors
    random,         // Random materials including transparency, reflection,
                    // and light emission
    shades_of_grey, // 255 shades of grey
    gradient,       // Gradient from red to yellow
    pastel          // Random pastel colors
};

enum class ShadingType
{
    none,
    diffuse,
    electron,
};

enum class CameraType
{
    perspective,
    stereo,
    orthographic,
    panoramic,
    clipped
};

enum class CameraStereoMode
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
typedef std::vector<Vector4f> ClipPlanes;

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
}

#endif // TYPES_H
