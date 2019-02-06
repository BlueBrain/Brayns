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
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/common/utils/base64/base64.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/RenderingParameters.h>
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

struct EnvironmentMapParam
{
    std::string filename;
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

struct ModelTransferFunction
{
    size_t id;
    TransferFunction transferFunction;
};
}

STATICJSON_DECLARE_ENUM(brayns::GeometryQuality,
                        {"low", brayns::GeometryQuality::low},
                        {"medium", brayns::GeometryQuality::medium},
                        {"high", brayns::GeometryQuality::high});

STATICJSON_DECLARE_ENUM(brayns::ColorScheme,
                        {"none", brayns::ColorScheme::none},
                        {"by_id", brayns::ColorScheme::by_id},
                        {"protein_atoms", brayns::ColorScheme::protein_atoms},
                        {"protein_chains", brayns::ColorScheme::protein_chains},
                        {"protein_residues",
                         brayns::ColorScheme::protein_residues});

STATICJSON_DECLARE_ENUM(brayns::MemoryMode,
                        {"shared", brayns::MemoryMode::shared},
                        {"replicated", brayns::MemoryMode::replicated});

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
template <size_t M, typename T>
auto toArray(vmml::vector<M, T>& vec)
{
    return reinterpret_cast<std::array<T, M>*>(&(vec).array[0]);
}

template <typename T>
auto toArray(vmml::Quaternion<T>& quat)
{
    return reinterpret_cast<std::array<T, 4>*>(&(quat).array[0]);
}

template <size_t M, typename T>
auto toArray(std::vector<vmml::vector<M, T>>& vecVec)
{
    return reinterpret_cast<std::vector<std::array<T, M>>*>(&vecVec);
}

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

inline void init(brayns::ModelTransferFunction* s, ObjectHandler* h)
{
    h->add_property("id", &s->id);
    h->add_property("transfer_function", &s->transferFunction);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::GetInstances* g, ObjectHandler* h)
{
    h->add_property("id", &g->modelID);
    h->add_property("result_range", toArray(g->resultRange), Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SchemaParam* s, ObjectHandler* h)
{
    h->add_property("endpoint", &s->endpoint);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::EnvironmentMapParam* s, ObjectHandler* h)
{
    h->add_property("filename", &s->filename);
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
    h->add_property("loader_name", &s->_loaderName, Flags::Optional);
    h->add_property("loader_properties", &s->_loaderProperties,
                    Flags::Optional);
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
    h->add_property("size", toArray(s->size));
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
    h->add_property("position", toArray(p->pos));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Camera* c, ObjectHandler* h)
{
    h->add_property("orientation", toArray(c->_orientation), Flags::Optional);
    h->add_property("position", toArray(c->_position), Flags::Optional);
    h->add_property("target", toArray(c->_target), Flags::Optional);
    h->add_property("current", &c->_currentType, Flags::Optional);
    static auto types = c->getTypes();
    h->add_property("types", &types, Flags::IgnoreRead | Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ImageGenerator::ImageBase64* i, ObjectHandler* h)
{
    h->add_property("data", &i->data);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ColorMap* t, ObjectHandler* h)
{
    h->add_property("name", &t->name, Flags::Optional);
    h->add_property("colors", toArray(t->colors));
}

inline void init(brayns::TransferFunction* t, ObjectHandler* h)
{
    h->add_property("range", toArray(t->_valuesRange), Flags::Optional);
    h->add_property("opacity_curve", toArray(t->_controlPoints),
                    Flags::Optional);
    h->add_property("colormap", &t->_colorMap, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Boxd* b, ObjectHandler* h)
{
    h->add_property("min", toArray(b->_min));
    h->add_property("max", toArray(b->_max));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Material* m, ObjectHandler* h)
{
    h->add_property("name", &m->_name, Flags::Optional);
    h->add_property("diffuse_color", toArray(m->_diffuseColor),
                    Flags::Optional);
    h->add_property("specular_color", toArray(m->_specularColor),
                    Flags::Optional);
    h->add_property("specular_exponent", &m->_specularExponent,
                    Flags::Optional);
    h->add_property("reflection_index", &m->_reflectionIndex, Flags::Optional);
    h->add_property("opacity", &m->_opacity, Flags::Optional);
    h->add_property("refraction_index", &m->_refractionIndex, Flags::Optional);
    h->add_property("light_emission", &m->_emission, Flags::Optional);
    h->add_property("glossiness", &m->_glossiness, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Transformation* g, ObjectHandler* h)
{
    h->add_property("translation", toArray(g->_translation));
    h->add_property("scale", toArray(g->_scale));
    h->add_property("rotation", toArray(g->_rotation));
    h->add_property("rotation_center", toArray(g->_rotationCenter),
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
    h->add_property("loader_name", &g->_loaderName, Flags::Optional);
    h->add_property("loader_properties", &g->_loaderProperties,
                    Flags::Optional);

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

inline void init(brayns::ApplicationParameters* a, ObjectHandler* h)
{
    h->add_property("engine", &a->_engine, Flags::IgnoreRead | Flags::Optional);
    h->add_property("jpeg_compression", &a->_jpegCompression, Flags::Optional);
    h->add_property("image_stream_fps", &a->_imageStreamFPS, Flags::Optional);
    h->add_property("viewport", toArray(a->_windowSize), Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::RenderingParameters* r, ObjectHandler* h)
{
    h->add_property("accumulation", &r->_accumulation, Flags::Optional);
    h->add_property("background_color", toArray(r->_backgroundColor),
                    Flags::Optional);
    h->add_property("current", &r->_renderer, Flags::Optional);
    h->add_property("head_light", &r->_headLight, Flags::Optional);
    h->add_property("max_accum_frames", &r->_maxAccumFrames, Flags::Optional);
    h->add_property("samples_per_pixel", &r->_spp, Flags::Optional);
    h->add_property("subsampling", &r->_subsampling, Flags::Optional);
    h->add_property("types", &r->_renderers,
                    Flags::IgnoreRead | Flags::Optional);
    h->add_property("variance_threshold", &r->_varianceThreshold,
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::VolumeParameters* v, ObjectHandler* h)
{
    h->add_property("volume_dimensions", toArray(v->_dimensions),
                    Flags::Optional);
    h->add_property("volume_element_spacing", toArray(v->_elementSpacing),
                    Flags::Optional);
    h->add_property("volume_offset", toArray(v->_offset), Flags::Optional);

    h->add_property("gradient_shading", &v->_gradientShading, Flags::Optional);
    h->add_property("single_shade", &v->_singleShade, Flags::Optional);
    h->add_property("pre_integration", &v->_preIntegration, Flags::Optional);
    h->add_property("adaptive_max_sampling_rate", &v->_adaptiveMaxSamplingRate,
                    Flags::Optional);
    h->add_property("adaptive_sampling", &v->_adaptiveSampling,
                    Flags::Optional);
    h->add_property("sampling_rate", &v->_samplingRate, Flags::Optional);
    h->add_property("specular", toArray(v->_specular), Flags::Optional);
    h->add_property("clip_box", &v->_clipBox, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::AnimationParameters* a, ObjectHandler* h)
{
    h->add_property("frame_count", &a->_numFrames, Flags::Optional);
    h->add_property("current", &a->_current, Flags::Optional);
    h->add_property("delta", &a->_delta, Flags::Optional);
    h->add_property("dt", &a->_dt, Flags::Optional);
    h->add_property("playing", &a->_playing, Flags::Optional);
    h->add_property("unit", &a->_unit, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::LoaderInfo* a, ObjectHandler* h)
{
    h->add_property("name", &a->name);
    h->add_property("extensions", &a->extensions);
    h->add_property("properties", &a->properties);
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
std::string to_json(const brayns::PropertyMap& obj);

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

template <>
inline std::string to_json(const brayns::ModelProperties& props)
{
    const auto jsonOriginal = staticjson::to_json_string(props);

    const std::string propertiesJson =
        "\"properties\":" + to_json(props.properties);

    const auto result =
        brayns::replaceFirstOccurrence(jsonOriginal, "\"properties\":{}",
                                       propertiesJson);

    return result;
}

template <typename T>
inline std::string modelBinaryParamsToJson(const T& params)
{
    const auto jsonOriginal = staticjson::to_json_string(params);

    const std::string propertiesJson =
        "\"loader_properties\":" + to_json(params.getLoaderProperties());

    const auto result =
        brayns::replaceFirstOccurrence(jsonOriginal, "\"loader_properties\":{}",
                                       propertiesJson);

    return result;
}

template <>
inline std::string to_json(const brayns::ModelParams& params)
{
    return modelBinaryParamsToJson(params);
}

template <>
inline std::string to_json(const brayns::BinaryParam& params)
{
    return modelBinaryParamsToJson(params);
}

template <class T>
inline bool from_json(T& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), &obj, nullptr);
}
template <>
inline bool from_json(brayns::Vector2d& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), toArray(obj), nullptr);
}

brayns::PropertyMap jsonToPropertyMap(const std::string& json);

template <typename T>
inline bool modelBinaryParamsFromJson(T& params, const std::string& json)
{
    using namespace rapidjson;
    Document document;
    document.Parse(json.c_str());

    if (document.HasMember("loader_properties"))
    {
        auto& loaderProperties = document["loader_properties"];
        Document propertyDoc;
        propertyDoc.CopyFrom(loaderProperties, propertyDoc.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        propertyDoc.Accept(writer);

        params.setLoaderProperties(jsonToPropertyMap(buffer.GetString()));

        // Clear loader_properties to be able to parse rest of values
        loaderProperties.SetObject();
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    const auto str = buffer.GetString();
    return staticjson::from_json_string(str, &params, nullptr);
}

template <>
inline bool from_json(brayns::BinaryParam& params, const std::string& json)
{
    return modelBinaryParamsFromJson(params, json);
}

template <>
inline bool from_json(brayns::ModelParams& params, const std::string& json)
{
    return modelBinaryParamsFromJson(params, json);
}
