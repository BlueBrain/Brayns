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
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/RenderingParameters.h>
#include <brayns/parameters/SceneParameters.h>
#include <brayns/parameters/StreamParameters.h>
#include <brayns/parameters/VolumeParameters.h>
#include <brayns/tasks/UploadBinaryTask.h>
#include <brayns/tasks/errors.h>
#include <brayns/version.h>

#include "ImageGenerator.h"
#include "SnapshotTask.h"
#include "base64/base64.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include "staticjson/staticjson.hpp"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

STATICJSON_DECLARE_ENUM(brayns::StereoMode, {"none", brayns::StereoMode::none},
                        {"left", brayns::StereoMode::left},
                        {"right", brayns::StereoMode::right},
                        {"side_by_side", brayns::StereoMode::side_by_side});

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

STATICJSON_DECLARE_ENUM(brayns::SceneEnvironment,
                        {"none", brayns::SceneEnvironment::none},
                        {"ground", brayns::SceneEnvironment::ground},
                        {"wall", brayns::SceneEnvironment::wall},
                        {"bounding_box",
                         brayns::SceneEnvironment::bounding_box});

STATICJSON_DECLARE_ENUM(brayns::MemoryMode,
                        {"shared", brayns::MemoryMode::shared},
                        {"replicated", brayns::MemoryMode::replicated});

STATICJSON_DECLARE_ENUM(
    brayns::RendererType, {"default", brayns::RendererType::default_},
    {"proximity", brayns::RendererType::proximity},
    {"simulation", brayns::RendererType::simulation},
    {"particle", brayns::RendererType::particle},
    {"geometry_normals", brayns::RendererType::geometryNormals},
    {"shading_normals", brayns::RendererType::shadingNormals},
    {"scientific_visualization",
     brayns::RendererType::scientificvisualization});

STATICJSON_DECLARE_ENUM(brayns::ShadingType,
                        {"none", brayns::ShadingType::none},
                        {"diffuse", brayns::ShadingType::diffuse},
                        {"electron", brayns::ShadingType::electron});

STATICJSON_DECLARE_ENUM(brayns::EngineType,
                        {"ospray", brayns::EngineType::ospray},
                        {"optix", brayns::EngineType::optix});

// c-array to std.array: https://stackoverflow.com/questions/11205186
#define Vector2uiArray(vec) \
    reinterpret_cast<std::array<unsigned, 2>*>(&(vec).array[0])
#define Vector3uiArray(vec) \
    reinterpret_cast<std::array<unsigned, 3>*>(&(vec).array[0])
#define Vector2fArray(vec) \
    reinterpret_cast<std::array<float, 2>*>(&(vec).array[0])
#define Vector3fArray(vec) \
    reinterpret_cast<std::array<float, 3>*>(&(vec).array[0])

namespace staticjson
{
inline void init(brayns::BinaryParam* s, ObjectHandler* h)
{
    h->add_property("size", &s->size);
    h->add_property("type", &s->type);
    h->add_property("name", &s->name, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::BinaryError* s, ObjectHandler* h)
{
    h->add_property("index", &s->index);
    h->add_property("supported_types", &s->supportedTypes);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SnapshotParams* s, ObjectHandler* h)
{
    h->add_property("format", &s->format);
    h->add_property("quality", &s->quality, Flags::Optional);
    h->add_property("samples_per_pixel", &s->samplesPerPixel, Flags::Optional);
    h->add_property("size", Vector2uiArray(s->size));
    h->add_property("name", &s->name, Flags::Optional);
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
    h->add_property("position", Vector3fArray(p->pos));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ClipPlane* c, ObjectHandler* h)
{
    h->add_property("normal", Vector3fArray(*c));
    h->add_property("d", &c->array[3]);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Camera* c, ObjectHandler* h)
{
    h->add_property("origin", Vector3fArray(c->_position), Flags::Optional);
    h->add_property("look_at", Vector3fArray(c->_target), Flags::Optional);
    h->add_property("up", Vector3fArray(c->_up), Flags::Optional);
    h->add_property("field_of_view", &c->_fieldOfView, Flags::Optional);
    h->add_property("aperture", &c->_aperture, Flags::Optional);
    h->add_property("focal_length", &c->_focalLength, Flags::Optional);
    h->add_property("eye_separation", &c->_eyeSeparation, Flags::Optional);
    h->add_property("clip_planes", &c->_clipPlanes, Flags::Optional);
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
    h->add_property("range", Vector2fArray(t->getValuesRange()),
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

inline void init(brayns::Boxf* b, ObjectHandler* h)
{
    h->add_property("min", Vector3fArray(b->_min));
    h->add_property("max", Vector3fArray(b->_max));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Material* m, ObjectHandler* h)
{
    h->add_property("diffuse_color", Vector3fArray(m->_color), Flags::Optional);
    h->add_property("specular_color", Vector3fArray(m->_specularColor),
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

inline void init(brayns::Scene* s, ObjectHandler* h)
{
    h->add_property("bounds", &s->getWorldBounds(),
                    Flags::IgnoreRead | Flags::Optional);
    h->add_property("materials", &s->getMaterials());
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
                    Vector2fArray(c->simulationValuesRange), Flags::Optional);
    h->add_property("histogram_size", &c->simulationHistogramSize,
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ConnectivityConfiguration* c, ObjectHandler* h)
{
    h->add_property("filename", &c->_connectivityFile, Flags::Optional);
    h->add_property("matrix_id", &c->_connectivityMatrixId, Flags::Optional);
    h->add_property("show_connections", &c->_connectivityShowConnections,
                    Flags::Optional);
    h->add_property("dimension_range",
                    Vector2uiArray(c->_connectivityDimensionRange),
                    Flags::Optional);
    h->add_property("scale", Vector3fArray(c->_connectivityScale),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::StreamParameters* s, ObjectHandler* h)
{
    h->add_property("host", &s->_host, Flags::Optional);
    h->add_property("enabled", &s->_enabled, Flags::Optional);
    h->add_property("id", &s->_id, Flags::Optional);
    h->add_property("port", &s->_port, Flags::Optional);
    h->add_property("compression", &s->_compression, Flags::Optional);
    h->add_property("quality", &s->_quality, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ApplicationParameters* a, ObjectHandler* h)
{
    h->add_property("jpeg_compression", &a->_jpegCompression, Flags::Optional);
    h->add_property("frame_export_folder", &a->_frameExportFolder,
                    Flags::Optional);
    h->add_property("synchronous_mode", &a->_synchronousMode, Flags::Optional);
    h->add_property("image_stream_fps", &a->_imageStreamFPS, Flags::Optional);
    h->add_property("viewport", Vector2uiArray(a->_windowSize),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::GeometryParameters* g, ObjectHandler* h)
{
    h->add_property("morphology_folder", &g->_morphologyFolder,
                    Flags::Optional);
    h->add_property("nest_circuit", &g->_NESTCircuit, Flags::Optional);
    h->add_property("nest_report", &g->_NESTReport, Flags::Optional);
    h->add_property("nest_cache_file", &g->_NESTCacheFile, Flags::Optional);
    h->add_property("pdb_file", &g->_pdbFile, Flags::Optional);
    h->add_property("pdb_folder", &g->_pdbFolder, Flags::Optional);
    h->add_property("xyzb_file", &g->_xyzbFile, Flags::Optional);
    h->add_property("mesh_folder", &g->_meshFolder, Flags::Optional);
    h->add_property("mesh_file", &g->_meshFile, Flags::Optional);
    h->add_property("load_cache_file", &g->_loadCacheFile, Flags::Optional);
    h->add_property("save_cache_file", &g->_saveCacheFile, Flags::Optional);
    h->add_property("radius_multiplier", &g->_radiusMultiplier,
                    Flags::Optional);
    h->add_property("radius_correction", &g->_radiusCorrection,
                    Flags::Optional);
    h->add_property("color_scheme", &g->_colorScheme, Flags::Optional);
    h->add_property("scene_environment", &g->_sceneEnvironment,
                    Flags::Optional);
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
    h->add_property("connectivity_configuration",
                    &g->_connectivityConfiguration, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::RenderingParameters* r, ObjectHandler* h)
{
    h->add_property("engine", &r->_engine, Flags::IgnoreRead | Flags::Optional);
    h->add_property("samples_per_pixel", &r->_spp, Flags::Optional);
    h->add_property("shader", &r->_renderer, Flags::Optional);
    h->add_property("shading", &r->_shading, Flags::Optional);
    h->add_property("shadows", &r->_shadows, Flags::Optional);
    h->add_property("soft_shadows", &r->_softShadows, Flags::Optional);
    h->add_property("ambient_occlusion", &r->_ambientOcclusionStrength,
                    Flags::Optional);
    h->add_property("ambient_occlusion_distance", &r->_ambientOcclusionDistance,
                    Flags::Optional);
    h->add_property("accumulation", &r->_accumulation, Flags::Optional);
    h->add_property("radiance", &r->_lightEmittingMaterials, Flags::Optional);
    h->add_property("epsilon", &r->_epsilon, Flags::Optional);
    h->add_property("head_light", &r->_headLight, Flags::Optional);
    h->add_property("variance_threshold", &r->_varianceThreshold,
                    Flags::Optional);
    h->add_property("max_accum_frames", &r->_maxAccumFrames, Flags::Optional);
    h->add_property("background_color", Vector3fArray(r->_backgroundColor),
                    Flags::Optional);
    h->add_property("detection_distance", &r->_detectionDistance,
                    Flags::Optional);
    h->add_property("detection_on_different_material",
                    &r->_detectionOnDifferentMaterial, Flags::Optional);
    h->add_property("detection_near_color",
                    Vector3fArray(r->_detectionNearColor), Flags::Optional);
    h->add_property("detection_far_color", Vector3fArray(r->_detectionFarColor),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
    h->add_property("samples_per_ray", &r->_spr, Flags::Optional);
    h->add_property("stereo_mode", &r->_stereoMode, Flags::Optional);
}

inline void init(brayns::SceneParameters* s, ObjectHandler* h)
{
    h->add_property("color_map_file", &s->_colorMapFilename, Flags::Optional);
    h->add_property("color_map_range", Vector2fArray(s->_colorMapRange),
                    Flags::Optional);
    h->add_property("environment_map", &s->_environmentMap, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::VolumeParameters* v, ObjectHandler* h)
{
    h->add_property("volume_folder", &v->_folder, Flags::Optional);
    h->add_property("volume_file", &v->_filename, Flags::Optional);
    h->add_property("volume_dimensions", Vector3uiArray(v->_dimensions),
                    Flags::Optional);
    h->add_property("volume_element_spacing", Vector3fArray(v->_elementSpacing),
                    Flags::Optional);
    h->add_property("volume_offset", Vector3fArray(v->_offset),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Histogram* hi, ObjectHandler* h)
{
    h->add_property("values", &hi->values);
    h->add_property("range", Vector2fArray(hi->range));
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
    return staticjson::to_pretty_json_string(obj);
}

template <>
inline std::string to_json(const brayns::Version& obj)
{
    return obj.toJSON();
}

template <class T>
inline bool from_json(T& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), &obj, nullptr);
}
template <>
inline bool from_json(brayns::Vector2f& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), Vector2fArray(obj),
                                        nullptr);
}
