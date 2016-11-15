/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <memory>
#include <map>
#include <vector>
#include <cstdint>

typedef ::int64_t int64;
typedef ::uint64_t uint64;
typedef ::int32_t int32;
typedef ::uint32_t uint32;
typedef ::int16_t int16;
typedef ::uint16_t uint16;
typedef ::int8_t int8;
typedef ::uint8_t uint8;
typedef ::int64_t index_t;

namespace brayns
{

// Forward declarations
class Brayns;
typedef std::shared_ptr< Brayns > BraynsPtr;

class Engine;
typedef std::shared_ptr< Engine > EnginePtr;
typedef std::map< std::string, EnginePtr > EngineMap;

class Scene;
typedef std::shared_ptr< Scene > ScenePtr;

class AbstractManipulator;
class Camera;
typedef std::shared_ptr< Camera > CameraPtr;

class TransferFunction;
typedef std::shared_ptr< TransferFunction > TransferFunctionPtr;

class Renderer;
typedef std::shared_ptr< Renderer > RendererPtr;
typedef std::map< std::string, RendererPtr > RendererMap;
typedef std::vector< RendererPtr > Renderers;

class FrameBuffer;
typedef std::shared_ptr< FrameBuffer > FrameBufferPtr;

class ApplicationParameters;
typedef std::shared_ptr< ApplicationParameters > ApplicationParametersPtr;

class GeometryParameters;
typedef std::shared_ptr< GeometryParameters > GeometryParametersPtr;

class RenderingParameters;
typedef std::shared_ptr< RenderingParameters > RenderingParametersPtr;

class VolumeParameters;
typedef std::shared_ptr< VolumeParameters > VolumeParametersPtr;

class ExtensionController;
typedef std::shared_ptr<ExtensionController> ExtensionControllerPtr;

class Geometry;
typedef std::vector< Geometry* > Geometries;

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
typedef std::shared_ptr<Material> MaterialPtr;
typedef std::vector<MaterialPtr> Materials;
typedef std::map<size_t, Materials> MaterialsMap;

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;
typedef std::map<std::string, Texture2DPtr> TexturesMap;

class Light;
typedef std::shared_ptr< Light > LightPtr;
typedef std::vector< LightPtr > Lights;
typedef std::map< size_t, LightPtr > LightsMap;

class DirectionalLight;
typedef std::shared_ptr< DirectionalLight > DirectionalLightPtr;

class PointLight;
typedef std::shared_ptr< PointLight > PointLightPtr;

class AbstractSimulationHandler;
typedef std::shared_ptr< AbstractSimulationHandler > AbstractSimulationHandlerPtr;

class CircuitSimulationHandler;
typedef std::shared_ptr< CircuitSimulationHandler > CircuitSimulationHandlerPtr;

class SpikeSimulationHandler;
typedef std::shared_ptr< SpikeSimulationHandler > SpikeSimulationHandlerPtr;

class VolumeHandler;
typedef std::shared_ptr< VolumeHandler > VolumeHandlerPtr;

typedef std::vector< std::string > strings;
typedef std::vector< float > floats;
typedef std::vector< int > ints;
typedef std::vector< unsigned int > uints;
typedef std::vector< uint8_t > uint8_ts;
typedef std::vector< uint16_t > uint16_ts;
typedef std::vector< uint32_t > uint32_ts;
typedef std::vector< uint64_t > uint64_ts;
typedef std::vector< size_t > size_ts;

class AbstractParameters;
class ApplicationParameters;
class GeometryParameters;
class RenderingParameters;
class SceneParameters;

class ParametersManager;
typedef std::shared_ptr< ParametersManager > ParametersManagerPtr;

class ExtensionPlugin;
typedef std::shared_ptr< ExtensionPlugin > ExtensionPluginPtr;
typedef std::vector< ExtensionPluginPtr > ExtensionPlugins;

class ExtensionPluginFactory;
typedef std::unique_ptr< ExtensionPluginFactory > ExtensionPluginFactoryPtr;

class ZeroEQPlugin;
typedef std::shared_ptr< ZeroEQPlugin > ZeroEQPluginPtr;

class DeflectPlugin;
typedef std::shared_ptr< DeflectPlugin > DeflectPluginPtr;

class KeyboardHandler;
typedef std::shared_ptr< KeyboardHandler > KeyboardHandlerPtr;

/** Define the frame buffer format */
enum FrameBufferFormat
{
  FBF_RGBA_NONE,
  FBF_RGBA_I8,
  FBF_RGB_I8,
  FBF_RGBA_F32,
};

/** Define the color scheme to be applied to the geometry */
enum ColorScheme
{
    CS_NONE,
    CS_PROTEIN_ATOMS,
    CS_PROTEIN_CHAINS,
    CS_PROTEIN_RESIDUES,
    CS_PROTEIN_BACKBONE,
    CS_NEURON_DEFAULT,
    CS_NEURON_BY_ID,
    CS_NEURON_BY_TYPE,
    CS_NEURON_BY_SEGMENT_TYPE,
};

/** Define the environment that is added to the default scene */
enum SceneEnvironment
{
    SE_NONE = 0,
    SE_GROUND,
    SE_WALL,
    SE_BOUNDING_BOX
};

/** Define light types */
enum LightType
{
    LT_POINT = 0,
    LT_DIRECTIONAL,
};

/** Geometry quality */
enum GeometryQuality
{
    GQ_FAST,
    GQ_QUALITY,
    GQ_MAX_QUALITY
};

/** Morphology element types */
enum MorphologySectionType
{
    MST_UNDEFINED = 0x00,
    MST_SOMA = 0x01,
    MST_AXON = 0x02,
    MST_DENDRITE = 0x04,
    MST_APICAL_DENDRITE = 0x08,
    MST_ALL = 0xff
};

/** Transfer function attributes */
enum TransferFunctionAttribute
{
    TF_UNDEFINED,
    TF_RED,
    TF_GREEN,
    TF_BLUE,
    TF_ALPHA,
    TF_EMISSION
};

/** Extension parameters */
struct ExtensionParameters
{
    ParametersManagerPtr parametersManager;
    EnginePtr engine;
};

/** Some 'special' materials are used by Brayns to acomplish specific features
 *  such as skyboxes.
 */
const size_t NO_MATERIAL = -1;
const size_t NB_MAX_MATERIALS = 200;
const size_t NB_SYSTEM_MATERIALS = 4;
const size_t MATERIAL_SYSTEM = NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS - 1;
const size_t MATERIAL_SKYBOX = MATERIAL_SYSTEM + 0;
const size_t MATERIAL_BOUNDING_BOX = MATERIAL_SYSTEM + 1;
const size_t MATERIAL_SIMULATION = MATERIAL_SYSTEM + 2;
const size_t MATERIAL_INVISIBLE = MATERIAL_SYSTEM + 3;
const std::string TEXTURE_NAME_SKYBOX = "SKYBOX";
const std::string TEXTURE_NAME_SIMULATION = "SIMULATION";

/** Defines how materials should be created */
enum MaterialType
{
    MT_DEFAULT,        // Random colors
    MT_RANDOM,         // Random materials including transparency, reflection,
                       // and light emission
    MT_SHADES_OF_GREY, // 255 shades of grey
    MT_GRADIENT,       // Gradient from red to yellow
    MT_PASTEL_COLORS,  // Random pastel colors
};

enum class ShadingType
{
    none,
    diffuse,
    electron,
};

enum MeshQuality
{
    MQ_FAST = 0,
    MQ_QUALITY,
    MQ_MAX_QUALITY
};

enum MorphologyLayoutType
{
    ML_NONE = 0,
    ML_GRID,
};

enum CameraType
{
    CT_UNDEFINED = 0,
    CT_PERSPECTIVE,
    CT_STEREO,
    CT_ORTHOGRAPHIC,
    CT_PANORAMIC
};

enum class TimestampMode
{
    unchanged,
    modulo,
    bounded
};

}

#endif // TYPES_H
