/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#include <brayns/common/Statistics.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/common/utils/base64/base64.h>
#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/RenderingParameters.h>
#include <brayns/parameters/SceneParameters.h>
#include <brayns/parameters/StreamParameters.h>
#include <brayns/parameters/VolumeParameters.h>
#include <brayns/tasks/AddModelFromBlobTask.h>
#include <brayns/tasks/errors.h>
#include <brayns/version.h>

#include "ImageGenerator.h"
#include "SnapshotTask.h"

#include "jsonUtils.h"

namespace brayns
{
struct GetInstances
{
    size_t modelID;
    Vector2ui resultRange;
};

struct SchemaParam
{
    std::string endpoint;
};

struct ObjectID
{
    size_t id;
};

struct ModelProperties
{
    size_t id;
    PropertyMap properties;
};
}

STATICJSON_DECLARE_ENUM(brayns::GeometryQuality,
                        {"low", brayns::GeometryQuality::low},
                        {"medium", brayns::GeometryQuality::medium},
                        {"high", brayns::GeometryQuality::high});

STATICJSON_DECLARE_ENUM(brayns::MorphologySectionType,
                        {"soma", brayns::MorphologySectionType::soma},
                        {"axon", brayns::MorphologySectionType::axon},
                        {"dendrite", brayns::MorphologySectionType::dendrite},
                        {"apical_dendrite",
                         brayns::MorphologySectionType::apical_dendrite},
                        {"all", brayns::MorphologySectionType::all});

STATICJSON_DECLARE_ENUM(
    brayns::ColorScheme, {"none", brayns::ColorScheme::none},
    {"neuron_by_id", brayns::ColorScheme::neuron_by_id},
    {"neuron_by_type", brayns::ColorScheme::neuron_by_type},
    {"neuron_by_segment_type", brayns::ColorScheme::neuron_by_segment_type},
    {"neuron_by_layer", brayns::ColorScheme::neuron_by_layer},
    {"neuron_by_mtype", brayns::ColorScheme::neuron_by_mtype},
    {"neuron_by_etype", brayns::ColorScheme::neuron_by_etype},
    {"neuron_by_target", brayns::ColorScheme::neuron_by_target},
    {"protein_by_id", brayns::ColorScheme::protein_by_id},
    {"protein_atoms", brayns::ColorScheme::protein_atoms},
    {"protein_chains", brayns::ColorScheme::protein_chains},
    {"protein_residues", brayns::ColorScheme::protein_residues});

STATICJSON_DECLARE_ENUM(brayns::MemoryMode,
                        {"shared", brayns::MemoryMode::shared},
                        {"replicated", brayns::MemoryMode::replicated});

STATICJSON_DECLARE_ENUM(brayns::EngineType,
                        {"ospray", brayns::EngineType::ospray},
                        {"optix", brayns::EngineType::optix});

STATICJSON_DECLARE_ENUM(brayns::TextureType,
                        {"diffuse", brayns::TextureType::TT_DIFFUSE},
                        {"normals", brayns::TextureType::TT_NORMALS},
                        {"bump", brayns::TextureType::TT_BUMP},
                        {"specular", brayns::TextureType::TT_SPECULAR},
                        {"emissive", brayns::TextureType::TT_EMISSIVE},
                        {"opacity", brayns::TextureType::TT_OPACITY},
                        {"reflection", brayns::TextureType::TT_REFLECTION},
                        {"refraction", brayns::TextureType::TT_REFRACTION},
                        {"occlusion", brayns::TextureType::TT_OCCLUSION});

// c-array to std.array: https://stackoverflow.com/questions/11205186
#define Vector2uiArray(vec) \
    reinterpret_cast<std::array<unsigned, 2>*>(&(vec).array[0])
#define Vector3uiArray(vec) \
    reinterpret_cast<std::array<unsigned, 3>*>(&(vec).array[0])
#define Vector2dArray(vec) \
    reinterpret_cast<std::array<double, 2>*>(&(vec).array[0])
#define Vector3dArray(vec) \
    reinterpret_cast<std::array<double, 3>*>(&(vec).array[0])
#define Vector4dArray(vec) \
    reinterpret_cast<std::array<double, 4>*>(&(vec).array[0])

namespace staticjson
{
inline void init(brayns::PropertyMap* /*g*/, ObjectHandler* h)
{
    // beware that this should only be used for creating JSON RPC schema which
    // uses PropertyMap as an argument and a oneOf list is not possible. To
    // properly serialize a property map to/from JSON, use from_json/to_json
    // from jsonPropertyMap.h directly.
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ObjectID* s, ObjectHandler* h)
{
    h->add_property("id", &s->id);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ModelProperties* s, ObjectHandler* h)
{
    h->add_property("id", &s->id);
    h->add_property("properties", &s->properties);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::GetInstances* g, ObjectHandler* h)
{
    h->add_property("id", &g->modelID);
    h->add_property("result_range", Vector2uiArray(g->resultRange),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SchemaParam* s, ObjectHandler* h)
{
    h->add_property("endpoint", &s->endpoint);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Chunk* c, ObjectHandler* h)
{
    h->add_property("id", &c->id, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::BinaryParam* s, ObjectHandler* h)
{
    h->add_property("bounding_box", &s->_boundingBox, Flags::Optional);
    h->add_property("chunks_id", &s->chunksID);
    h->add_property("name", &s->_name, Flags::Optional);
    h->add_property("path", &s->_path);
    h->add_property("size", &s->size);
    h->add_property("transformation", &s->_transformation, Flags::Optional);
    h->add_property("type", &s->type);
    h->add_property("visible", &s->_visible, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::BinaryError* s, ObjectHandler* h)
{
    h->add_property("supported_types", &s->supportedTypes);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SnapshotParams* s, ObjectHandler* h)
{
    h->add_property("animation_parameters", &s->animParams, Flags::Optional);
    h->add_property("camera", &s->camera, Flags::Optional);
    h->add_property("format", &s->format);
    h->add_property("name", &s->name, Flags::Optional);
    h->add_property("quality", &s->quality, Flags::Optional);
    h->add_property("renderer", &s->renderingParams, Flags::Optional);
    h->add_property("samples_per_pixel", &s->samplesPerPixel, Flags::Optional);
    h->add_property("size", Vector2uiArray(s->size));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Statistics* s, ObjectHandler* h)
{
    h->add_property("fps", &s->_fps);
    h->add_property("scene_size_in_bytes", &s->_sceneSizeInBytes);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Renderer::PickResult* p, ObjectHandler* h)
{
    h->add_property("hit", &p->hit);
    h->add_property("position", Vector3dArray(p->pos));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Camera* c, ObjectHandler* h)
{
    h->add_property("look_at", Vector3dArray(c->_target), Flags::Optional);
    h->add_property("origin", Vector3dArray(c->_position), Flags::Optional);
    h->add_property("current", &c->_currentType, Flags::Optional);
    static auto types = c->getTypes();
    h->add_property("types", &types, Flags::IgnoreRead | Flags::Optional);
    h->add_property("up", Vector3dArray(c->_up), Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::FrameBuffer* f, ObjectHandler* h)
{
    static brayns::Vector2ui frameSize;
    static std::string diffuse, depth;

    frameSize = f->getSize();
    f->map();
    diffuse = base64_encode(f->getColorBuffer(),
                            frameSize.x() * frameSize.y() * f->getColorDepth());

    if (f->getDepthBuffer())
    {
        depth =
            base64_encode(reinterpret_cast<const uint8_t*>(f->getDepthBuffer()),
                          frameSize.x() * frameSize.y() * sizeof(float));
    }
    f->unmap();

    h->add_property("width", &frameSize[0]);
    h->add_property("height", &frameSize[1]);
    h->add_property("diffuse", &diffuse);
    h->add_property("depth", &depth, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ImageGenerator::ImageBase64* i, ObjectHandler* h)
{
    h->add_property("data", &i->data);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::TransferFunction* t, ObjectHandler* h)
{
    h->add_property("range", Vector2dArray(t->getValuesRange()),
                    Flags::Optional);
    h->add_property("diffuse",
                    reinterpret_cast<std::vector<std::array<float, 4>>*>(
                        &t->getDiffuseColors()),
                    Flags::Optional);
    h->add_property("emission",
                    reinterpret_cast<std::vector<std::array<float, 3>>*>(
                        &t->getEmissionIntensities()),
                    Flags::Optional);
    h->add_property("contribution", &t->getContributions(), Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Boxd* b, ObjectHandler* h)
{
    h->add_property("min", Vector3dArray(b->_min));
    h->add_property("max", Vector3dArray(b->_max));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Material* m, ObjectHandler* h)
{
    h->add_property("name", &m->_name, Flags::Optional);
    h->add_property("diffuse_color", Vector3dArray(m->_diffuseColor),
                    Flags::Optional);
    h->add_property("specular_color", Vector3dArray(m->_specularColor),
                    Flags::Optional);
    h->add_property("specular_exponent", &m->_specularExponent,
                    Flags::Optional);
    h->add_property("reflection_index", &m->_reflectionIndex, Flags::Optional);
    h->add_property("opacity", &m->_opacity, Flags::Optional);
    h->add_property("refraction_index", &m->_refractionIndex, Flags::Optional);
    h->add_property("light_emission", &m->_emission, Flags::Optional);
    h->add_property("glossiness", &m->_glossiness, Flags::Optional);
    h->add_property("cast_simulation_data", &m->_castSimulationData,
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Transformation* g, ObjectHandler* h)
{
    h->add_property("translation", Vector3dArray(g->_translation));
    h->add_property("scale", Vector3dArray(g->_scale));
    h->add_property("rotation", Vector4dArray(g->_rotation));
    h->add_property("rotation_center", Vector3dArray(g->_rotationCenter),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ModelInstance* i, ObjectHandler* h)
{
    h->add_property("model_id", &i->_modelID);
    h->add_property("instance_id", &i->_instanceID);
    h->add_property("bounding_box", &i->_boundingBox, Flags::Optional);
    h->add_property("transformation", &i->_transformation, Flags::Optional);
    h->add_property("visible", &i->_visible, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ModelParams* g, ObjectHandler* h)
{
    h->add_property("bounding_box", &g->_boundingBox, Flags::Optional);
    h->add_property("name", &g->_name, Flags::Optional);
    h->add_property("path", &g->_path);
    h->add_property("transformation", &g->_transformation, Flags::Optional);
    h->add_property("visible", &g->_visible, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ModelDescriptor* g, ObjectHandler* h)
{
    h->add_property("bounding_box", &g->_boundingBox, Flags::Optional);
    h->add_property("bounds", &g->_bounds, Flags::Optional);
    h->add_property("id", &g->_modelID);
    h->add_property("metadata", &g->_metadata, Flags::Optional);
    h->add_property("name", &g->_name, Flags::Optional);
    h->add_property("path", &g->_path, Flags::Optional);
    h->add_property("transformation", &g->_transformation, Flags::Optional);
    h->add_property("visible", &g->_visible, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ClipPlane* g, ObjectHandler* h)
{
    h->add_property("id", &g->_id);
    h->add_property("plane", &g->_plane);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Scene* s, ObjectHandler* h)
{
    h->add_property("bounds", &s->_bounds, Flags::IgnoreRead | Flags::Optional);
    h->add_property("models", &s->_modelDescriptors,
                    Flags::Optional | Flags::IgnoreRead);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::MorphologyLayout* m, ObjectHandler* h)
{
    h->add_property("nb_columns", &m->nbColumns);
    h->add_property("vertical_spacing", &m->verticalSpacing);
    h->add_property("horizontal_spacing", &m->horizontalSpacing);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::CircuitConfiguration* c, ObjectHandler* h)
{
    h->add_property("circuit_config_file", &c->circuitConfigFile,
                    Flags::Optional);
    h->add_property("density", &c->density, Flags::Optional);
    h->add_property("bounding_box", &c->boundingBox, Flags::Optional);
    h->add_property("mesh_filename_pattern", &c->meshFilenamePattern,
                    Flags::Optional);
    h->add_property("mesh_folder", &c->meshFolder, Flags::Optional);
    h->add_property("mesh_transformation", &c->meshTransformation,
                    Flags::Optional);
    h->add_property("use_simulation_model", &c->useSimulationModel,
                    Flags::Optional);
    h->add_property("targets", &c->targets, Flags::Optional);
    h->add_property("report", &c->report, Flags::Optional);
    h->add_property("start_simulation_time", &c->startSimulationTime,
                    Flags::Optional);
    h->add_property("end_simulation_time", &c->endSimulationTime,
                    Flags::Optional);
    h->add_property("simulation_step", &c->simulationStep, Flags::Optional);
    h->add_property("simulation_values_range",
                    Vector2dArray(c->simulationValuesRange), Flags::Optional);
    h->add_property("histogram_size", &c->simulationHistogramSize,
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::StreamParameters* s, ObjectHandler* h)
{
    h->add_property("host", &s->_host);
    h->add_property("enabled", &s->_enabled, Flags::Optional);
    h->add_property("id", &s->_id, Flags::Optional);
    h->add_property("port", &s->_port, Flags::Optional);
    h->add_property("compression", &s->_compression, Flags::Optional);
    h->add_property("quality", &s->_quality, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ApplicationParameters* a, ObjectHandler* h)
{
    h->add_property("engine", &a->_engine, Flags::IgnoreRead | Flags::Optional);
    h->add_property("jpeg_compression", &a->_jpegCompression, Flags::Optional);
    h->add_property("frame_export_folder", &a->_frameExportFolder,
                    Flags::Optional);
    h->add_property("synchronous_mode", &a->_synchronousMode, Flags::Optional);
    h->add_property("image_stream_fps", &a->_imageStreamFPS, Flags::Optional);
    h->add_property("viewport", Vector2dArray(a->_windowSize), Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::GeometryParameters* g, ObjectHandler* h)
{
    h->add_property("nest_circuit", &g->_NESTCircuit, Flags::Optional);
    h->add_property("nest_report", &g->_NESTReport, Flags::Optional);
    h->add_property("nest_cache_file", &g->_NESTCacheFile, Flags::Optional);
    h->add_property("load_cache_file", &g->_loadCacheFile, Flags::Optional);
    h->add_property("save_cache_file", &g->_saveCacheFile, Flags::Optional);
    h->add_property("radius_multiplier", &g->_radiusMultiplier,
                    Flags::Optional);
    h->add_property("radius_correction", &g->_radiusCorrection,
                    Flags::Optional);
    h->add_property("color_scheme", &g->_colorScheme, Flags::Optional);
    h->add_property("geometry_quality", &g->_geometryQuality, Flags::Optional);
    h->add_property("morphology_section_types", &g->_morphologySectionTypes,
                    Flags::Optional);
    h->add_property("morphology_layout", &g->_morphologyLayout,
                    Flags::Optional);
    h->add_property("molecular_system_config", &g->_molecularSystemConfig,
                    Flags::Optional);
    h->add_property("metaballs_grid_size", &g->_metaballsGridSize,
                    Flags::Optional);
    h->add_property("metaballs_threshold", &g->_metaballsThreshold,
                    Flags::Optional);
    h->add_property("metaballs_samples_from_soma",
                    &g->_metaballsSamplesFromSoma, Flags::Optional);
    h->add_property("memory_mode", &g->_memoryMode, Flags::Optional);
    h->add_property("circuit_configuration", &g->_circuitConfiguration,
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::RenderingParameters* r, ObjectHandler* h)
{
    h->add_property("accumulation", &r->_accumulation, Flags::Optional);
    h->add_property("background_color", Vector3dArray(r->_backgroundColor),
                    Flags::Optional);
    h->add_property("current", &r->_renderer, Flags::Optional);
    h->add_property("head_light", &r->_headLight, Flags::Optional);
    h->add_property("max_accum_frames", &r->_maxAccumFrames, Flags::Optional);
    h->add_property("samples_per_pixel", &r->_spp, Flags::Optional);
    h->add_property("types", &r->_renderers,
                    Flags::IgnoreRead | Flags::Optional);
    h->add_property("variance_threshold", &r->_varianceThreshold,
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SceneParameters* s, ObjectHandler* h)
{
    h->add_property("color_map_file", &s->_colorMapFilename, Flags::Optional);
    h->add_property("color_map_range", Vector2dArray(s->_colorMapRange),
                    Flags::Optional);
    h->add_property("environment_map", &s->_environmentMap, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::VolumeParameters* v, ObjectHandler* h)
{
    h->add_property("volume_dimensions", Vector3uiArray(v->_dimensions),
                    Flags::Optional);
    h->add_property("volume_element_spacing", Vector3dArray(v->_elementSpacing),
                    Flags::Optional);
    h->add_property("volume_offset", Vector3dArray(v->_offset),
                    Flags::Optional);

    h->add_property("gradient_shading", &v->_gradientShading, Flags::Optional);
    h->add_property("single_shade", &v->_singleShade, Flags::Optional);
    h->add_property("pre_integration", &v->_preIntegration, Flags::Optional);
    h->add_property("adaptive_max_sampling_rate", &v->_adaptiveMaxSamplingRate,
                    Flags::Optional);
    h->add_property("adaptive_sampling", &v->_adaptiveSampling,
                    Flags::Optional);
    h->add_property("sampling_rate", &v->_samplingRate, Flags::Optional);
    h->add_property("specular", Vector3dArray(v->_specular), Flags::Optional);
    h->add_property("clip_box", &v->_clipBox, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Histogram* hi, ObjectHandler* h)
{
    h->add_property("values", &hi->values);
    h->add_property("range", Vector2dArray(hi->range));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::AnimationParameters* a, ObjectHandler* h)
{
    h->add_property("start", &a->_start, Flags::Optional);
    h->add_property("end", &a->_end, Flags::Optional);
    h->add_property("current", &a->_current, Flags::Optional);
    h->add_property("delta", &a->_delta, Flags::Optional);
    h->add_property("dt", &a->_dt, Flags::Optional);
    h->add_property("unit", &a->_unit, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}
}

// for rockets::jsonrpc
template <class T>
inline std::string to_json(const T& obj)
{
    return staticjson::to_json_string(obj);
}

template <>
inline std::string to_json(const brayns::Version& obj)
{
    return obj.toJSON();
}

template <>
inline std::string to_json(const brayns::Scene& scene)
{
    auto lock = scene.acquireReadAccess();
    return staticjson::to_json_string(scene);
}

template <class T>
inline bool from_json(T& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), &obj, nullptr);
}
template <>
inline bool from_json(brayns::Vector2d& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), Vector2dArray(obj),
                                        nullptr);
}
