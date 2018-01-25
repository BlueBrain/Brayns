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

#include "RocketsPlugin.h"

#include <brayns/Brayns.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/version.h>

#include <fstream>

#include "SDK.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

namespace
{
const std::string ENDPOINT_API_VERSION = "v1/";
const std::string ENDPOINT_CAMERA = "camera";
const std::string ENDPOINT_DATA_SOURCE = "data-source";
const std::string ENDPOINT_FRAME_BUFFERS = "frame-buffers";
const std::string ENDPOINT_SCENE = "scene";
const std::string ENDPOINT_APP_PARAMS = "application-parameters";
const std::string ENDPOINT_GEOMETRY_PARAMS = "geometry-parameters";
const std::string ENDPOINT_RENDERING_PARAMS = "rendering-parameters";
const std::string ENDPOINT_SCENE_PARAMS = "scene-parameters";
const std::string ENDPOINT_VOLUME_PARAMS = "volume-parameters";
const std::string ENDPOINT_SIMULATION_HISTOGRAM = "simulation-histogram";
const std::string ENDPOINT_VOLUME_HISTOGRAM = "volume-histogram";
const std::string ENDPOINT_VERSION = "version";
const std::string ENDPOINT_PROGRESS = "progress";
const std::string ENDPOINT_FRAME = "frame";
const std::string ENDPOINT_IMAGE_JPEG = "image-jpeg";
const std::string ENDPOINT_MATERIAL_LUT = "material-lut";
const std::string ENDPOINT_CIRCUIT_CONFIG_BUILDER = "circuit-config-builder";
const std::string ENDPOINT_STREAM = "stream";
const std::string ENDPOINT_STREAM_TO = "stream-to";

const std::string JSON_TYPE = "application/json";

const size_t NB_MAX_MESSAGES = 20; // Maximum number of network messages to read
                                   // between each rendering loop

// JSON for websocket text messages
std::string _buildJsonMessage(const std::string& event, const std::string data,
                              const bool error)
{
    rapidjson::Document message(rapidjson::kObjectType);

    rapidjson::Value eventJson;
    eventJson.SetString(event.c_str(), event.length(), message.GetAllocator());
    message.AddMember("event", eventJson, message.GetAllocator());

    rapidjson::Document dataJson(rapidjson::kObjectType);
    dataJson.Parse(data.c_str(), data.length());
    if (error)
        message.AddMember("error", dataJson.GetObject(),
                          message.GetAllocator());
    else
        message.AddMember("data", dataJson.GetObject(), message.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    message.Accept(writer);
    return sb.GetString();
}

std::string buildJsonMessage(const std::string& event, const std::string data)
{
    return _buildJsonMessage(event, data, false);
}

std::string buildJsonErrorMessage(const std::string& event,
                                  const std::string data)
{
    return _buildJsonMessage(event, data, true);
}

std::string hyphenatedToCamelCase(const std::string& scoreString)
{
    std::string camelString = scoreString;

    for (size_t x = 0; x < camelString.length(); x++)
    {
        if (camelString[x] == '-')
        {
            std::string tempString = camelString.substr(x + 1, 1);

            transform(tempString.begin(), tempString.end(), tempString.begin(),
                      toupper);

            camelString.erase(x, 2);
            camelString.insert(x, tempString);
        }
    }
    camelString[0] = toupper(camelString[0]);
    return camelString;
}

// get JSON schema from JSON-serializable object
template <class T>
std::string getSchema(T& obj, const std::string& title)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    auto schema = staticjson::export_json_schema(&obj);
    schema.AddMember(rapidjson::StringRef("title"),
                     rapidjson::StringRef(title.c_str()),
                     schema.GetAllocator());
    schema.Accept(writer);

    return buffer.GetString();
}
}

namespace brayns
{
template <class T>
inline std::string to_json(const T& obj)
{
    return staticjson::to_pretty_json_string(obj);
}
template <>
inline std::string to_json(const Engine::Progress& obj)
{
    std::lock_guard<std::mutex> lock(obj.mutex);
    return staticjson::to_pretty_json_string(obj);
}
template <>
inline std::string to_json(const Version& obj)
{
    return obj.toJSON();
}
template <class T, class F>
inline bool from_json(T& obj, const std::string& json, F postUpdateFunc = [] {})
{
    staticjson::ParseStatus status;
    const auto success =
        staticjson::from_json_string(json.c_str(), &obj, &status);
    if (success)
    {
        obj.markModified();
        if (std::function<void(T&)>(postUpdateFunc))
            postUpdateFunc(obj);
    }
    else
        BRAYNS_ERROR << status.description() << std::endl;
    return success;
}

RocketsPlugin::RocketsPlugin(ParametersManager& parametersManager)
    : ExtensionPlugin()
    , _parametersManager(parametersManager)
    , _imageGenerator(parametersManager.getApplicationParameters())
{
    _setupHTTPServer();
}

RocketsPlugin::~RocketsPlugin()
{
}

void RocketsPlugin::_onNewEngine()
{
    _handle(ENDPOINT_CAMERA, _engine->getCamera());
    _handleGET(ENDPOINT_PROGRESS, _engine->getProgress());
    _handleGET(ENDPOINT_FRAME_BUFFERS, _engine->getFrameBuffer(),
               [](const FrameBuffer&) { return false; });
    _handle(ENDPOINT_MATERIAL_LUT, _engine->getScene().getTransferFunction());

    _handleGET(ENDPOINT_SCENE, _engine->getScene(), [&](const Scene& scene) {
        return _engine->isReady() && scene.getModified();
    });
    _handlePUT(ENDPOINT_SCENE, _engine->getScene(),
               [](Scene& scene) { scene.commitMaterials(Action::update); });

    _handleSimulationHistogram();
    _handleVolumeHistogram();
}

bool RocketsPlugin::run(EnginePtr engine, KeyboardHandler&,
                        AbstractManipulator&)
{
    if (!_httpServer)
        return true;

    if (_engine != engine)
    {
        _engine = engine;
        _onNewEngine();
    }

    try
    {
        _broadcastWebsocketMessages();

        // In the case of interactions with Jupyter notebooks, HTTP messages are
        // received in a blocking and sequential manner, meaning that the
        // subscriber never has more than one message in its queue. In other
        // words, only one message is processed between each rendering loop. The
        // following code allows the processing of several messages and performs
        // rendering after NB_MAX_MESSAGES reads.
        for (size_t i = 0; i < NB_MAX_MESSAGES; ++i)
            _httpServer->process(0);
    }
    catch (const std::exception& exc)
    {
        BRAYNS_ERROR << "Error while handling HTTP/websocket messages: "
                     << exc.what() << std::endl;
    }

    return true;
}

void RocketsPlugin::_broadcastWebsocketMessages()
{
    if (_httpServer->getConnectionCount() == 0)
        return;

    for (auto& message : _wsBroadcasts)
        message.second();
}

rockets::ws::Response RocketsPlugin::_processWebsocketMessage(
    const std::string& message)
{
    try
    {
        rapidjson::Document jsonData;
        jsonData.Parse(message.c_str());
        auto eventStr = jsonData["event"].GetString();
        if (!eventStr)
            return buildJsonErrorMessage("exception", "Missing event type");

        const std::string event{eventStr};
        auto i = _wsIncoming.find(event);
        if (i == _wsIncoming.end())
            return buildJsonErrorMessage(event, "Unknown websocket event");

        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        if (!jsonData["data"].Accept(writer))
            return buildJsonErrorMessage(event, "Could not parse data");

        auto dataStr = sb.GetString();
        if (!eventStr)
            return buildJsonErrorMessage(event, "Data is empty");

        std::string response;
        if (!i->second(dataStr, response))
            return buildJsonErrorMessage(event, "Could not update object");

        // re-broadcast to all other clients
        return rockets::ws::Response{response, rockets::ws::Recipient::others};
    }
    catch (const std::exception& exc)
    {
        BRAYNS_ERROR << "Error in websocket message handling: " << exc.what()
                     << std::endl;
        return buildJsonErrorMessage("exception", exc.what());
    }
}

void RocketsPlugin::_handleApplicationParams()
{
    auto& params = _parametersManager.getApplicationParameters();
    auto postUpdate = [this](ApplicationParameters& params_) {
        if (params_.getFrameExportFolder().empty())
            _engine->resetFrameNumber();
    };
    _handleGET(ENDPOINT_APP_PARAMS, params);
    _handlePUT(ENDPOINT_APP_PARAMS, params, postUpdate);
}

void RocketsPlugin::_handleGeometryParams()
{
    auto& params = _parametersManager.getGeometryParameters();
    auto postUpdate = [this](GeometryParameters&) {
        if (_engine->isReady())
            _engine->buildScene();
    };
    _handleGET(ENDPOINT_GEOMETRY_PARAMS, params);
    _handlePUT(ENDPOINT_GEOMETRY_PARAMS, params, postUpdate);
}

void RocketsPlugin::_handleSimulationHistogram()
{
    Histogram tmp;
    _handleSchema(ENDPOINT_SIMULATION_HISTOGRAM, tmp);

    using namespace rockets::http;
    _httpServer->handle(Method::GET,
                        ENDPOINT_API_VERSION + ENDPOINT_SIMULATION_HISTOGRAM,
                        [this](const Request&) {
                            auto simulationHandler =
                                _engine->getScene().getSimulationHandler();
                            if (!simulationHandler)
                                return make_ready_response(Code::NOT_SUPPORTED);
                            auto histo = simulationHandler->getHistogram();
                            return make_ready_response(Code::OK, to_json(histo),
                                                       JSON_TYPE);
                        });
}

void RocketsPlugin::_handleVolumeHistogram()
{
    Histogram tmp;
    _handleSchema(ENDPOINT_VOLUME_HISTOGRAM, tmp);

    using namespace rockets::http;
    _httpServer->handle(Method::GET,
                        ENDPOINT_API_VERSION + ENDPOINT_VOLUME_HISTOGRAM,
                        [this](const Request&) {
                            auto volumeHandler =
                                _engine->getScene().getVolumeHandler();
                            if (!volumeHandler)
                                return make_ready_response(Code::NOT_SUPPORTED);
                            auto histo = volumeHandler->getHistogram();
                            return make_ready_response(Code::OK, to_json(histo),
                                                       JSON_TYPE);
                        });
}

void RocketsPlugin::_handleVolumeParams()
{
    auto& params = _parametersManager.getVolumeParameters();
    auto postUpdate = [this](VolumeParameters&) {
        if (_engine->isReady())
            _engine->buildScene();
    };
    _handleGET(ENDPOINT_VOLUME_PARAMS, params);
    _handlePUT(ENDPOINT_VOLUME_PARAMS, params, postUpdate);
}

void RocketsPlugin::_setupHTTPServer()
{
    try
    {
        _httpServer.reset(
            new rockets::Server{_getHttpInterface(), "rockets", 0});
        BRAYNS_INFO << "Registering http handlers on " << _httpServer->getURI()
                    << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << "HTTP could not be initialized: '" << e.what() << "'"
                     << std::endl;
        return;
    }

    _setupWebsocket();

    _handleVersion();
    _handleStreaming();
    _handleImageJPEG();

    _handleApplicationParams();
    _handleGeometryParams();
    _handleVolumeParams();

    _handle(ENDPOINT_RENDERING_PARAMS,
            _parametersManager.getRenderingParameters());
    _handle(ENDPOINT_SCENE_PARAMS, _parametersManager.getSceneParameters());

    _handle(ENDPOINT_FRAME, _parametersManager.getAnimationParameters());

    _httpServer->handle(rockets::http::Method::GET,
                        ENDPOINT_API_VERSION + ENDPOINT_CIRCUIT_CONFIG_BUILDER,
                        std::bind(&RocketsPlugin::_handleCircuitConfigBuilder,
                                  this, std::placeholders::_1));
}

void RocketsPlugin::_setupWebsocket()
{
    _httpServer->handleOpen([this]() {
        std::vector<rockets::ws::Response> responses;
        for (auto& i : _wsOutgoing)
            responses.push_back({i.second(), rockets::ws::Recipient::sender,
                                 rockets::ws::Format::text});

        if (_engine->isReady())
        {
            const auto image =
                _imageGenerator.createJPEG(_engine->getFrameBuffer());
            if (image.size > 0)
            {
                std::string message;
                message.assign((const char*)image.data.get(), image.size);
                responses.push_back({message, rockets::ws::Recipient::sender,
                                     rockets::ws::Format::binary});
            }
        }
        return responses;
    });
    _httpServer->handleText(std::bind(&RocketsPlugin::_processWebsocketMessage,
                                      this, std::placeholders::_1));
}

std::string RocketsPlugin::_getHttpInterface() const
{
    const auto& params = _parametersManager.getApplicationParameters();
    const auto& args = params.arguments();
    for (int i = 0; i < (int)args.size() - 1; ++i)
    {
        if (args[i] == "--http-server" || args[i] == "--zeroeq-http-server")
            return args[i + 1];
    }
    return std::string();
}

template <class T>
void RocketsPlugin::_handle(const std::string& endpoint, T& obj)
{
    _handleGET(endpoint, obj);
    _handlePUT(endpoint, obj);
}

template <class T, class F>
void RocketsPlugin::_handleGET(const std::string& endpoint, T& obj,
                               F modifiedFunc)
{
    using namespace rockets::http;

    _httpServer->handle(Method::GET, ENDPOINT_API_VERSION + endpoint,
                        [&obj](const Request&) {
                            return make_ready_response(Code::OK, to_json(obj),
                                                       JSON_TYPE);
                        });

    _handleSchema(endpoint, obj);

    auto jsonMessageFunc = [&obj, endpoint] {
        return buildJsonMessage(endpoint, to_json(obj));
    };

    _wsOutgoing[endpoint] = jsonMessageFunc;

    _wsBroadcasts[endpoint] = [this, &obj, jsonMessageFunc, modifiedFunc] {
        if (modifiedFunc(obj))
            _httpServer->broadcastText(jsonMessageFunc());
    };
}

template <class T>
void RocketsPlugin::_handlePUT(const std::string& endpoint, T& obj)
{
    _handlePUT(endpoint, obj, std::function<void(T&)>());
}

template <class T, class F>
void RocketsPlugin::_handlePUT(const std::string& endpoint, T& obj,
                               F postUpdateFunc)
{
    using namespace rockets::http;
    _httpServer->handle(Method::PUT, ENDPOINT_API_VERSION + endpoint,
                        [&obj, postUpdateFunc](const Request& req) {
                            return make_ready_response(
                                from_json(obj, req.body, postUpdateFunc)
                                    ? Code::OK
                                    : Code::BAD_REQUEST);
                        });

    _handleSchema(endpoint, obj);

    _wsIncoming[endpoint] = [&obj, postUpdateFunc](const std::string& data,
                                                   std::string& newData) {
        if (from_json(obj, data, postUpdateFunc))
        {
            newData = to_json(obj);
            return true;
        }
        return false;
    };
}

template <class T>
void RocketsPlugin::_handleSchema(const std::string& endpoint, T& obj)
{
    using namespace rockets::http;
    _httpServer->handle(Method::GET,
                        ENDPOINT_API_VERSION + endpoint + "/schema",
                        [&obj, endpoint](const Request&) {
                            return make_ready_response(
                                Code::OK,
                                getSchema(obj, hyphenatedToCamelCase(endpoint)),
                                JSON_TYPE);
                        });
}

void RocketsPlugin::_remove(const std::string& endpoint)
{
    _httpServer->remove(ENDPOINT_API_VERSION + endpoint);
    _httpServer->remove(ENDPOINT_API_VERSION + endpoint + "/schema");
    _wsOutgoing.erase(endpoint);
    _wsIncoming.erase(endpoint);
    _wsBroadcasts.erase(endpoint);
}

void RocketsPlugin::_handleVersion()
{
    static brayns::Version version;
    using namespace rockets::http;
    _httpServer->handleGET(ENDPOINT_API_VERSION + ENDPOINT_VERSION, version);
    _httpServer->handle(Method::GET,
                        ENDPOINT_API_VERSION + ENDPOINT_VERSION + "/schema",
                        [&](const Request&) {
                            return make_ready_response(Code::OK,
                                                       version.getSchema(),
                                                       JSON_TYPE);
                        });
}

void RocketsPlugin::_handleStreaming()
{
#if BRAYNS_USE_DEFLECT
    _handle(ENDPOINT_STREAM, _parametersManager.getStreamParameters());
    _handlePUT(ENDPOINT_STREAM_TO, _parametersManager.getStreamParameters());
#else
    _handleGET(ENDPOINT_STREAM, _parametersManager.getStreamParameters());
    using namespace rockets::http;
    auto respondNotImplemented = [](const Request&) {
        const auto message = "Brayns was not compiled with streaming support";
        return make_ready_response(Code::NOT_IMPLEMENTED, message);
    };
    _httpServer->handle(Method::PUT, ENDPOINT_STREAM, respondNotImplemented);
    _httpServer->handle(Method::PUT, ENDPOINT_STREAM_TO, respondNotImplemented);
#endif
}

void RocketsPlugin::_handleImageJPEG()
{
    using namespace rockets::http;

    _httpServer->handle(Method::GET, ENDPOINT_API_VERSION + ENDPOINT_IMAGE_JPEG,
                        [&](const Request&) {
                            if (!_engine->isReady())
                                return make_ready_response(Code::NOT_SUPPORTED);
                            auto obj = _imageGenerator.createJPEG(
                                _engine->getFrameBuffer());
                            if (obj.size == 0)
                                return make_ready_response(Code::BAD_REQUEST);
                            return make_ready_response(Code::OK, to_json(obj),
                                                       JSON_TYPE);
                        });

    _httpServer->handle(
        Method::GET, ENDPOINT_API_VERSION + ENDPOINT_IMAGE_JPEG + "/schema",
        [&](const Request&) {
            ImageGenerator::ImageJPEG obj;
            return make_ready_response(Code::OK,
                                       getSchema(obj, hyphenatedToCamelCase(
                                                          ENDPOINT_IMAGE_JPEG)),
                                       JSON_TYPE);
        });

    _wsBroadcasts[ENDPOINT_IMAGE_JPEG] = [this] {
        if (_engine->isReady() && _engine->getRenderer().hasNewImage())
        {
            const auto fps = _parametersManager.getApplicationParameters()
                                 .getImageStreamFPS();
            if (_timer.elapsed() < 1.f / fps)
                return;

            _timer.restart();

            const auto image =
                _imageGenerator.createJPEG(_engine->getFrameBuffer());
            if (image.size > 0)
                _httpServer->broadcastBinary((const char*)image.data.get(),
                                             image.size);
        }
    };
}

std::future<rockets::http::Response> RocketsPlugin::_handleCircuitConfigBuilder(
    const rockets::http::Request& request)
{
    using namespace rockets::http;

    const auto& params = _parametersManager.getApplicationParameters();
    const auto filename = params.getTmpFolder() + "/BlueConfig";
    if (_writeBlueConfigFile(filename, request.query))
    {
        const std::string body = "{\"filename\":\"" + filename + "\"}";
        return make_ready_response(Code::OK, body, JSON_TYPE);
    }
    return make_ready_response(Code::SERVICE_UNAVAILABLE);
}

bool RocketsPlugin::_writeBlueConfigFile(
    const std::string& filename,
    const std::map<std::string, std::string>& params)
{
    std::ofstream blueConfig(filename);
    if (!blueConfig.good())
        return false;

    std::map<std::string, std::string> dictionary = {
        {"morphology_folder", "MorphologyPath"}, {"mvd_file", "CircuitPath"}};

    blueConfig << "Run Default" << std::endl << "{" << std::endl;
    for (const auto& kv : params)
    {
        if (dictionary.find(kv.first) == dictionary.end())
        {
            BRAYNS_ERROR << "BlueConfigBuilder: Unknown parameter " << kv.first
                         << std::endl;
            continue;
        }
        blueConfig << dictionary[kv.first] << " " << kv.second << std::endl;
    }
    blueConfig << "}" << std::endl;
    blueConfig.close();
    return true;
}
}
