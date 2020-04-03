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
#include <brayns/common/light/Light.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/common/utils/base64/base64.h>
#include <brayns/common/utils/stringUtils.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engineapi/Camera.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/engineapi/Material.h>
#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Renderer.h>
#include <brayns/engineapi/Scene.h>
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

struct VideoStreamParam
{
    bool enabled{false};
    uint32_t kbps{5000};

    bool operator==(const VideoStreamParam& rhs) const
    {
        return enabled == rhs.enabled && kbps == rhs.kbps;
    }

    bool operator!=(const VideoStreamParam& rhs) const
    {
        return !(*this == rhs);
    }
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

struct RPCLight
{
    size_t id;
    std::string type;
    PropertyMap properties;
};

struct InputPath
{
    std::string path;
};

struct FileStats
{
    std::string type;
    long sizeBytes;
    int error;
    std::string message;
};

struct FileType
{
    std::string type;
    int error;
    std::string message;
};

struct FileContent
{
    std::string content;
    int error;
    std::string message;
};

struct FileRoot
{
    std::string root;
};

struct DirectoryFiles
{
    strings names;
    uint64_ts sizes;
};

struct DirectoryFileList
{
    DirectoryFiles files;
    strings dirs;
    int error;
    std::string message;
};

struct ImageStreamingMethod
{
    std::string type;
};

struct ExitLaterSchedule
{
    uint32_t minutes;
};

} // namespace brayns

STATICJSON_DECLARE_ENUM(brayns::GeometryQuality,
                        {"low", brayns::GeometryQuality::low},
                        {"medium", brayns::GeometryQuality::medium},
                        {"high", brayns::GeometryQuality::high});

STATICJSON_DECLARE_ENUM(
    brayns::ProteinColorScheme, {"none", brayns::ProteinColorScheme::none},
    {"by_id", brayns::ProteinColorScheme::by_id},
    {"protein_atoms", brayns::ProteinColorScheme::protein_atoms},
    {"protein_chains", brayns::ProteinColorScheme::protein_chains},
    {"protein_residues", brayns::ProteinColorScheme::protein_residues});

STATICJSON_DECLARE_ENUM(brayns::MemoryMode,
                        {"shared", brayns::MemoryMode::shared},
                        {"replicated", brayns::MemoryMode::replicated});

STATICJSON_DECLARE_ENUM(brayns::TextureType,
                        {"diffuse", brayns::TextureType::diffuse},
                        {"normals", brayns::TextureType::normals},
                        {"bump", brayns::TextureType::bump},
                        {"specular", brayns::TextureType::specular},
                        {"emissive", brayns::TextureType::emissive},
                        {"opacity", brayns::TextureType::opacity},
                        {"reflection", brayns::TextureType::reflection},
                        {"refraction", brayns::TextureType::refraction},
                        {"occlusion", brayns::TextureType::occlusion});

// c-array to std.array: https://stackoverflow.com/questions/11205186
template <size_t M, typename T>
auto toArray(glm::vec<M, T>& vec)
{
    return reinterpret_cast<std::array<T, M>*>(glm::value_ptr(vec));
}

template <typename T>
auto toArray(glm::tquat<T>& quat)
{
    return reinterpret_cast<std::array<T, 4>*>(glm::value_ptr(quat));
}

template <size_t M, typename T>
auto toArray(std::vector<glm::vec<M, T>>& vecVec)
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
    h->add_property("result_range", toArray<2, uint32_t>(g->resultRange),
                    Flags::Optional);
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

inline void init(brayns::VideoStreamParam* s, ObjectHandler* h)
{
    h->add_property("enabled", &s->enabled, Flags::Optional);
    h->add_property("kbps", &s->kbps, Flags::Optional);
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
    h->add_property("size", toArray<2, uint32_t>(s->size));
    h->add_property("filePath", &s->filePath, Flags::Optional);
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
    h->add_property("position", toArray<3, double>(p->pos));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Camera* c, ObjectHandler* h)
{
    h->add_property("orientation", toArray(c->_orientation), Flags::Optional);
    h->add_property("position", toArray<3, double>(c->_position),
                    Flags::Optional);
    h->add_property("target", toArray<3, double>(c->_target), Flags::Optional);
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
    h->add_property("colors", toArray<3, float>(t->colors));
}

inline void init(brayns::TransferFunction* t, ObjectHandler* h)
{
    h->add_property("range", toArray<2, double>(t->_valuesRange),
                    Flags::Optional);
    h->add_property("opacity_curve", toArray<2, double>(t->_controlPoints),
                    Flags::Optional);
    h->add_property("colormap", &t->_colorMap, Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Boxd* b, ObjectHandler* h)
{
    h->add_property("min", toArray<3, double>(b->_min));
    h->add_property("max", toArray<3, double>(b->_max));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::Material* m, ObjectHandler* h)
{
    h->add_property("name", &m->_name, Flags::Optional);
    h->add_property("diffuse_color", toArray<3, double>(m->_diffuseColor),
                    Flags::Optional);
    h->add_property("specular_color", toArray<3, double>(m->_specularColor),
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
    h->add_property("translation", toArray<3, double>(g->_translation));
    h->add_property("scale", toArray<3, double>(g->_scale));
    h->add_property("rotation", toArray(g->_rotation));
    h->add_property("rotation_center", toArray<3, double>(g->_rotationCenter),
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
    h->add_property("viewport", toArray<2, double>(a->_windowSize),
                    Flags::Optional);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::RenderingParameters* r, ObjectHandler* h)
{
    h->add_property("accumulation", &r->_accumulation, Flags::Optional);
    h->add_property("background_color", toArray<3, double>(r->_backgroundColor),
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
    h->add_property("volume_dimensions", toArray<3, uint32_t>(v->_dimensions),
                    Flags::Optional);
    h->add_property("volume_element_spacing",
                    toArray<3, double>(v->_elementSpacing), Flags::Optional);
    h->add_property("volume_offset", toArray<3, double>(v->_offset),
                    Flags::Optional);

    h->add_property("gradient_shading", &v->_gradientShading, Flags::Optional);
    h->add_property("single_shade", &v->_singleShade, Flags::Optional);
    h->add_property("pre_integration", &v->_preIntegration, Flags::Optional);
    h->add_property("adaptive_max_sampling_rate", &v->_adaptiveMaxSamplingRate,
                    Flags::Optional);
    h->add_property("adaptive_sampling", &v->_adaptiveSampling,
                    Flags::Optional);
    h->add_property("sampling_rate", &v->_samplingRate, Flags::Optional);
    h->add_property("specular", toArray<3, double>(v->_specular),
                    Flags::Optional);
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

inline void init(brayns::Light* a, ObjectHandler* h)
{
    h->add_property("color", toArray<3, double>(a->_color));
    h->add_property("intensity", &a->_intensity);
    h->add_property("is_visible", &a->_isVisible);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::DirectionalLight* a, ObjectHandler* h)
{
    init(static_cast<brayns::Light*>(a), h);
    h->add_property("direction", toArray<3, double>(a->_direction));
    h->add_property("angularDiameter", &a->_angularDiameter);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SphereLight* a, ObjectHandler* h)
{
    init(static_cast<brayns::Light*>(a), h);
    h->add_property("position", toArray<3, double>(a->_position));
    h->add_property("radius", &a->_radius);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::QuadLight* a, ObjectHandler* h)
{
    init(static_cast<brayns::Light*>(a), h);
    h->add_property("position", toArray<3, double>(a->_position));
    h->add_property("edge1", toArray<3, double>(a->_edge1));
    h->add_property("edge2", toArray<3, double>(a->_edge2));
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::SpotLight* a, ObjectHandler* h)
{
    init(static_cast<brayns::Light*>(a), h);
    h->add_property("position", toArray<3, double>(a->_position));
    h->add_property("direction", toArray<3, double>(a->_direction));
    h->add_property("openingAngle", &a->_openingAngle);
    h->add_property("penumbraAngle", &a->_penumbraAngle);
    h->add_property("radius", &a->_radius);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::AmbientLight* a, ObjectHandler* h)
{
    init(static_cast<brayns::Light*>(a), h);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::RPCLight* a, ObjectHandler* h)
{
    h->add_property("id", &a->id);
    h->add_property("type", &a->type);
    h->add_property("properties", &a->properties);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::InputPath* a, ObjectHandler* h)
{
    h->add_property("path", &a->path);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::FileType* a, ObjectHandler* h)
{
    h->add_property("type", &a->type);
    h->add_property("error", &a->error);
    h->add_property("message", &a->message);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::FileContent* a, ObjectHandler* h)
{
    h->add_property("content", &a->content);
    h->add_property("error", &a->error);
    h->add_property("message", &a->message);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::FileRoot* a, ObjectHandler* h)
{
    h->add_property("root", &a->root);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::DirectoryFiles* a, ObjectHandler* h)
{
    h->add_property("names", &a->names);
    h->add_property("sizes", &a->sizes);
}

inline void init(brayns::DirectoryFileList* a, ObjectHandler* h)
{
    h->add_property("files", &a->files);
    h->add_property("dirs", &a->dirs);
    h->add_property("error", &a->error);
    h->add_property("message", &a->message);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ImageStreamingMethod* a, ObjectHandler* h)
{
    h->add_property("type", &a->type);
    h->set_flags(Flags::DisallowUnknownKey);
}

inline void init(brayns::ExitLaterSchedule* a, ObjectHandler* h)
{
    h->add_property("minutes", &a->minutes);
    h->set_flags(Flags::DisallowUnknownKey);
}

} // namespace staticjson

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

template <typename T>
inline std::string toJSONReplacePropertyMap(
    const T& params, const std::string& propertyMapName,
    const brayns::PropertyMap& properties)
{
    const auto jsonOriginal = staticjson::to_json_string(params);

    const std::string key = "\"" + propertyMapName + "\"";
    const std::string propertiesJson = key + ":" + to_json(properties);

    const auto result =
        brayns::string_utils::replaceFirstOccurrence(jsonOriginal, key + ":{}",
                                                     propertiesJson);
    return result;
}

template <>
inline std::string to_json(const brayns::ModelProperties& props)
{
    return toJSONReplacePropertyMap(props, "properties", props.properties);
}

template <>
inline std::string to_json(const brayns::ModelParams& params)
{
    return toJSONReplacePropertyMap(params, "loader_properties",
                                    params.getLoaderProperties());
}

template <>
inline std::string to_json(const brayns::BinaryParam& params)
{
    return toJSONReplacePropertyMap(params, "loader_properties",
                                    params.getLoaderProperties());
}

template <>
inline std::string to_json(const brayns::RPCLight& light)
{
    return toJSONReplacePropertyMap(light, "properties", light.properties);
}

template <class T>
inline bool from_json(T& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), &obj, nullptr);
}
template <>
inline bool from_json(brayns::Vector2d& obj, const std::string& json)
{
    return staticjson::from_json_string(json.c_str(), toArray<2, double>(obj),
                                        nullptr);
}

brayns::PropertyMap jsonToPropertyMap(const std::string& json);

template <typename T>
inline std::pair<bool, brayns::PropertyMap> fromJSONWithPropertyMap(
    T& params, const std::string& json, const std::string& propertyMapName)
{
    using namespace rapidjson;
    Document document;
    document.Parse(json.c_str());

    brayns::PropertyMap propertyMap;

    if (document.HasMember(propertyMapName.c_str()))
    {
        auto& loaderProperties = document[propertyMapName.c_str()];
        Document propertyDoc;
        propertyDoc.CopyFrom(loaderProperties, propertyDoc.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        propertyDoc.Accept(writer);

        propertyMap = jsonToPropertyMap(buffer.GetString());

        // Clear property map to be able to parse rest of values
        loaderProperties.SetObject();
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    const auto str = buffer.GetString();
    return {staticjson::from_json_string(str, &params, nullptr), propertyMap};
}

template <typename T>
inline bool modelBinaryParamsFromJson(T& params, const std::string& json)
{
    bool success;
    brayns::PropertyMap propertyMap;
    std::tie<bool, brayns::PropertyMap>(success, propertyMap) =
        fromJSONWithPropertyMap(params, json, "loader_properties");
    params.setLoaderProperties(propertyMap);
    return success;
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

template <>
inline bool from_json(brayns::RPCLight& light, const std::string& json)
{
    bool success;
    brayns::PropertyMap propertyMap;
    std::tie<bool, brayns::PropertyMap>(success, propertyMap) =
        fromJSONWithPropertyMap(light, json, "properties");
    light.properties = propertyMap;
    return success;
}
