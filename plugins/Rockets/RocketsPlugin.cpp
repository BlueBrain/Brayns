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

// needs to be before RocketsPlugin.h to make template instantiation for
// _handleRPC work
#include "jsonPropertyMap.h"

#include "RocketsPlugin.h"

#include <brayns/common/Timer.h>
#include <brayns/common/tasks/Task.h>
#include <brayns/common/utils/stringUtils.h>
#include <brayns/pluginapi/Plugin.h>

#include <brayns/tasks/AddModelFromBlobTask.h>
#include <brayns/tasks/AddModelTask.h>
#include <brayns/tasks/LoadModelFunctor.h>

#ifdef BRAYNS_USE_LIBUV
#include <uvw.hpp>
#endif

#include <rockets/jsonrpc/helpers.h>
#include <rockets/jsonrpc/server.h>
#include <rockets/server.h>

#include "BinaryRequests.h"
#include "ImageGenerator.h"
#include "Throttle.h"

#include <atomic>
#include <dirent.h>
#include <fstream>
#include <limits.h>
#include <unistd.h>

#include <sys/stat.h>

#ifdef BRAYNS_USE_FFMPEG
#include "encoder.h"
#endif

namespace
{
constexpr int64_t INTERACTIVE_THROTTLE = 1;
constexpr int64_t DEFAULT_THROTTLE = 50;
constexpr int64_t SLOW_THROTTLE = 750;

const int MODEL_NOT_FOUND = -12345;
const int INSTANCE_NOT_FOUND = -12346;
const int TASK_RESULT_TO_JSON_ERROR = -12347;
const int SCHEMA_RPC_ENDPOINT_NOT_FOUND = -12348;
const int PARAMETER_FROM_JSON_ERROR = -12349;
const int VIDEOSTREAMING_NOT_SUPPORTED = -12350;
const int VIDEOSTREAMING_NOT_ENABLED = -12351;

// REST PUT & GET, JSONRPC set-* notification, JSONRPC get-* request
const std::string ENDPOINT_ANIMATION_PARAMS = "animation-parameters";
const std::string ENDPOINT_APP_PARAMS = "application-parameters";
const std::string ENDPOINT_CAMERA = "camera";
const std::string ENDPOINT_CAMERA_PARAMS = "camera-params";
const std::string ENDPOINT_RENDERER = "renderer";
const std::string ENDPOINT_RENDERER_PARAMS = "renderer-params";
const std::string ENDPOINT_SCENE = "scene";
const std::string ENDPOINT_VOLUME_PARAMS = "volume-parameters";

// REST GET, JSONRPC get-* request
const std::string ENDPOINT_STATISTICS = "statistics";
const std::string ENDPOINT_VERSION = "version";

// JSONRPC async requests
const std::string METHOD_ADD_MODEL = "add-model";
const std::string METHOD_SNAPSHOT = "snapshot";
// METHOD_REQUEST_MODEL_UPLOAD from BinaryRequests.h

// JSONRPC synchronous requests
const std::string METHOD_ADD_CLIP_PLANE = "add-clip-plane";
const std::string METHOD_GET_CLIP_PLANES = "get-clip-planes";
const std::string METHOD_GET_ENVIRONMENT_MAP = "get-environment-map";
const std::string METHOD_GET_INSTANCES = "get-instances";
const std::string METHOD_GET_LOADERS = "get-loaders";
const std::string METHOD_GET_MODEL_PROPERTIES = "get-model-properties";
const std::string METHOD_GET_MODEL_TRANSFER_FUNCTION =
    "get-model-transfer-function";
const std::string METHOD_GET_VIDEOSTREAM = "get-videostream";
const std::string METHOD_IMAGE_JPEG = "image-jpeg";
const std::string METHOD_SET_STREAMING_METHOD = "image-streaming-mode";
const std::string METHOD_TRIGGER_JPEG_STREAM = "trigger-jpeg-stream";
const std::string METHOD_INSPECT = "inspect";
const std::string METHOD_MODEL_PROPERTIES_SCHEMA = "model-properties-schema";
const std::string METHOD_REMOVE_CLIP_PLANES = "remove-clip-planes";
const std::string METHOD_REMOVE_MODEL = "remove-model";
const std::string METHOD_SCHEMA = "schema";
const std::string METHOD_SET_ENVIRONMENT_MAP = "set-environment-map";
const std::string METHOD_SET_MODEL_PROPERTIES = "set-model-properties";
const std::string METHOD_SET_MODEL_TRANSFER_FUNCTION =
    "set-model-transfer-function";
const std::string METHOD_SET_VIDEOSTREAM = "set-videostream";
const std::string METHOD_UPDATE_CLIP_PLANE = "update-clip-plane";
const std::string METHOD_UPDATE_INSTANCE = "update-instance";
const std::string METHOD_UPDATE_MODEL = "update-model";
const std::string METHOD_GET_LIGHTS = "get-lights";
const std::string METHOD_ADD_LIGHT_SPHERE = "add-light-sphere";
const std::string METHOD_ADD_LIGHT_DIRECTIONAL = "add-light-directional";
const std::string METHOD_ADD_LIGHT_QUAD = "add-light-quad";
const std::string METHOD_ADD_LIGHT_SPOT = "add-light-spot";
const std::string METHOD_ADD_LIGHT_AMBIENT = "add-light-ambient";
const std::string METHOD_REMOVE_LIGHTS = "remove-lights";
const std::string METHOD_CLEAR_LIGHTS = "clear-lights";

const std::string METHOD_FS_EXISTS = "fs-exists";
const std::string METHOD_FS_GET_CONTENT = "fs-get-content";
const std::string METHOD_FS_GET_ROOT = "fs-get-root";
const std::string METHOD_FS_LIST_DIR = "fs-list-dir";

// JSONRPC notifications
const std::string METHOD_CHUNK = "chunk";
const std::string METHOD_QUIT = "quit";
const std::string METHOD_EXIT_LATER = "exit-later";
const std::string METHOD_RESET_CAMERA = "reset-camera";

const std::string LOADERS_SCHEMA = "loaders-schema";

const std::string JSON_TYPE = "application/json";

using Response = rockets::jsonrpc::Response;

std::string hyphenatedToCamelCase(const std::string& hyphenated)
{
    std::string camel = hyphenated;

    for (size_t x = 0; x < camel.length(); x++)
    {
        if (camel[x] == '-')
        {
            std::string tempString = camel.substr(x + 1, 1);

            transform(tempString.begin(), tempString.end(), tempString.begin(),
                      toupper);

            camel.erase(x, 2);
            camel.insert(x, tempString);
        }
    }
    camel[0] = toupper(camel[0]);
    return camel;
}

std::string getNotificationEndpointName(const std::string& endpoint)
{
    return "set-" + endpoint;
}

std::string getRequestEndpointName(const std::string& endpoint)
{
    return "get-" + endpoint;
}

const Response::Error VIDEOSTREAM_NOT_ENABLED_ERROR{
    "Brayns was not started with videostream support enabled",
    VIDEOSTREAMING_NOT_ENABLED};
const Response::Error VIDEOSTREAM_NOT_SUPPORTED_ERROR{
    "Brayns was not build with videostream support",
    VIDEOSTREAMING_NOT_SUPPORTED};
} // namespace

namespace brayns
{
template <class T, class PRE>
bool preUpdate(const std::string& json, PRE preUpdateFunc,
               typename std::enable_if<!std::is_abstract<T>::value>::type* = 0)
{
    if (std::function<bool(const T&)>(preUpdateFunc))
    {
        T temp;
        if (!staticjson::from_json_string(json.c_str(), &temp, nullptr))
            return false;
        if (!preUpdateFunc(temp))
            return false;
    }
    return true;
}

template <class T, class PRE>
bool preUpdate(const std::string&, PRE,
               typename std::enable_if<std::is_abstract<T>::value>::type* = 0)
{
    return true;
}

template <class T, class PRE, class POST>
inline bool from_json(
    T& obj, const std::string& json, PRE preUpdateFunc = [] {},
    POST postUpdateFunc = [] {})
{
    staticjson::ParseStatus status;

    if (!preUpdate<T>(json, preUpdateFunc))
        return false;

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

class RocketsPlugin::Impl : public ActionInterface
{
public:
    Impl(PluginAPI* api)
        : _engine(api->getEngine())
        , _parametersManager(api->getParametersManager())
    {
        _setupRocketsServer();
#ifdef BRAYNS_USE_LIBUV
        if (uvw::Loop::getDefault()->alive())
        {
            _processDelayedNotifies =
                uvw::Loop::getDefault()->resource<uvw::AsyncHandle>();
            _processDelayedNotifies->on<uvw::AsyncEvent>(
                [&](const auto&, auto&) { this->processDelayedNotifies(); });
        }
#endif
    }

    ~Impl()
    {
        // cancel all pending tasks
        decltype(_tasks) tasksToCancel;
        {
            std::lock_guard<std::mutex> lock(_tasksMutex);
            tasksToCancel = _tasks;
        }

        for (const auto& entry : tasksToCancel)
        {
            entry.first->cancel();
            entry.second->wait();
        }

#ifdef BRAYNS_USE_LIBUV
        if (_processDelayedNotifies)
            _processDelayedNotifies->close();
#endif

        if (_rocketsServer)
            _rocketsServer->setSocketListener(nullptr);

        for (auto i : _objects)
            i->clearModifiedCallback();
    }

    void preRender()
    {
        if (!_endpointsRegistered)
            _registerEndpoints();
        if (!_rocketsServer || !_manualProcessing)
            return;

        try
        {
            processDelayedNotifies();
            _rocketsServer->process(0);
        }
        catch (const std::exception& exc)
        {
            BRAYNS_ERROR << "Error while handling HTTP/websocket messages: "
                         << exc.what() << std::endl;
        }
    }

    void postRender()
    {
        if (!_rocketsServer || _rocketsServer->getConnectionCount() == 0)
            return;

        if (!_parametersManager.getApplicationParameters().useVideoStreaming())
        {
            if (_useControlledStream)
                _broadcastControlledImageJpeg();
            else
                _broadcastImageJpeg();
        }
#ifdef BRAYNS_USE_FFMPEG
        else
            _broadcastVideo();
#endif
    }

    void registerNotification(const RpcParameterDescription& desc,
                              const PropertyMap& input,
                              const std::function<void(PropertyMap)>& action)
    {
        _jsonrpcServer->connect(desc.methodName, [action,
                                                  this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            action(jsonToPropertyMap(request.message));
        });

        _handleSchema(desc.methodName,
                      buildJsonRpcSchemaNotifyPropertyMap(desc, input));
    }

    void registerNotification(const RpcDescription& desc,
                              const std::function<void()>& action)
    {
        _jsonrpcServer->connect(desc.methodName, [action,
                                                  this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            action();
        });

        _handleSchema(desc.methodName, buildJsonRpcSchemaNotify(desc));
    }

    void registerRequest(const RpcParameterDescription& desc,
                         const PropertyMap& input, const PropertyMap& output,
                         const std::function<PropertyMap(PropertyMap)>& action)
    {
        _bindEndpoint(desc.methodName, [name = desc.methodName,
                                        action](const auto& request) {
            try
            {
                return Response{
                    to_json(action(jsonToPropertyMap(request.message)))};
            }
            catch (...)
            {
                return Response{
                    Response::Error{"from_json for " + name + " failed",
                                    PARAMETER_FROM_JSON_ERROR}};
            }
        });

        _handleSchema(desc.methodName,
                      buildJsonRpcSchemaRequestPropertyMap(desc, input,
                                                           output));
    }

    void registerRequest(const RpcDescription& desc, const PropertyMap& output,
                         const std::function<PropertyMap()>& action)
    {
        _jsonrpcServer->bind(desc.methodName, [action,
                                               this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            return Response{to_json(action())};
        });

        _handleSchema(desc.methodName,
                      buildJsonRpcSchemaRequestPropertyMap(desc, output));
    }

    void _registerRequest(const std::string& name, const RetParamFunc& action)
    {
        _jsonrpcServer->bind(name, [action, this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            return Response{action(request.message)};
        });
    }

    void _registerRequest(const std::string& name, const RetFunc& action)
    {
        _jsonrpcServer->bind(name, [action, this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            return Response{action()};
        });
    }

    void _registerNotification(const std::string& name, const ParamFunc& action)
    {
        _jsonrpcServer->connect(name, [action, this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            action(request.message);
        });
    }

    void _registerNotification(const std::string& name, const VoidFunc& action)
    {
        _jsonrpcServer->connect(name, [action, this](const auto& request) {
            ScopedCurrentClient scope(this->_currentClientID, request.clientID);
            action();
        });
    }

    void processDelayedNotifies()
    {
        // call pending notifies from delayed throttle threads here as
        // notify() and process() are not threadsafe within Rockets.
        std::vector<std::function<void()>> delayedNotifies;
        {
            std::lock_guard<std::mutex> lock(_delayedNotifiesMutex);
            delayedNotifies = std::move(_delayedNotifies);
        }

        for (const auto& func : delayedNotifies)
            func();
    }

    void _setupRocketsServer()
    {
        try
        {
            const auto& appParams =
                _parametersManager.getApplicationParameters();
#ifdef BRAYNS_USE_LIBUV
            if (uvw::Loop::getDefault()->alive())
            {
                _rocketsServer = std::make_unique<rockets::Server>(
                    uv_default_loop(), appParams.getHttpServerURI(), "rockets");
                _manualProcessing = false;
            }
            else
#endif
                _rocketsServer = std::make_unique<rockets::Server>(
                    appParams.getHttpServerURI(), "rockets", 0);

            BRAYNS_INFO << "Rockets server running on "
                        << _rocketsServer->getURI() << std::endl;

            _jsonrpcServer = std::make_unique<JsonRpcServer>(*_rocketsServer);

            _parametersManager.getApplicationParameters().setHttpServerURI(
                _rocketsServer->getURI());
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << "Rockets server could not be initialized: '"
                         << e.what() << "'" << std::endl;
            return;
        }

        _setupWebsocket();
        _timer.start();
    }

    void _setupWebsocket()
    {
        _rocketsServer->handleClose([this](const uintptr_t clientID) {
            _binaryRequests.removeRequest(clientID);
            return std::vector<rockets::ws::Response>{};
        });

        _rocketsServer->handleBinary(std::bind(&BinaryRequests::processMessage,
                                               std::ref(_binaryRequests),
                                               std::placeholders::_1));
    }

    void _delayedNotify(const std::function<void()>& notify)
    {
        {
            std::lock_guard<std::mutex> lock(_delayedNotifiesMutex);
            _delayedNotifies.push_back(notify);
        }

#ifdef BRAYNS_USE_LIBUV
        if (_processDelayedNotifies)
        {
            // dispatch delayed notify from throttle thread to main thread
            // (where the default loop runs) as notify() and process() are not
            // threadsafe within Rockets.
            _processDelayedNotifies->send();
        }
#endif
    }

    void _rebroadcast(const std::string& endpoint, const std::string& message,
                      const std::set<uintptr_t>& filter)
    {
        _delayedNotify([&, message, filter] {
            if (_rocketsServer->getConnectionCount() > 1)
            {
                try
                {
                    const auto& msg =
                        rockets::jsonrpc::makeNotification(endpoint, message);
                    _rocketsServer->broadcastText(msg, filter);
                }
                catch (const std::exception& e)
                {
                    BRAYNS_ERROR
                        << "Error rebroadcasting notification: " << e.what()
                        << std::endl;
                }
            }
        });
    }

    // Utilty to change current client while we are handling a message to skip
    // notification to the current client and to trigger a delayed notify to
    // avoid a deadlock which happens when sending a message from within a
    // message handler.
    struct ScopedCurrentClient
    {
        ScopedCurrentClient(uintptr_t& currentClientID, const uintptr_t newID)
            : _currentClientID(currentClientID)
        {
            _currentClientID = newID;
        }

        ~ScopedCurrentClient() { _currentClientID = NO_CURRENT_CLIENT; }

    private:
        uintptr_t& _currentClientID;
    };

    void _bindEndpoint(const std::string& method,
                       rockets::jsonrpc::ResponseCallback action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);
            return action(request);
        });
    }

    template <typename Params, typename RetVal>
    void _bindEndpoint(const std::string& method,
                       std::function<RetVal(Params)> action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            Params params;
            if (!::from_json(params, request.message))
                return Response::invalidParams();
            try
            {
                const auto& ret = action(std::move(params));
                return Response{to_json(ret)};
            }
            catch (const rockets::jsonrpc::response_error& e)
            {
                return Response{Response::Error{e.what(), e.code}};
            }
        });
    }

    template <typename RetVal>
    void _bindEndpoint(const std::string& method,
                       std::function<RetVal()> action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            try
            {
                const auto& ret = action();
                return Response{to_json(ret)};
            }
            catch (const rockets::jsonrpc::response_error& e)
            {
                return Response{Response::Error{e.what(), e.code}};
            }
        });
    }

    template <typename Params>
    void _bindEndpoint(const std::string& method,
                       std::function<void(Params)> action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            Params params;
            if (!::from_json(params, request.message))
                return Response::invalidParams();
            action(std::move(params));
            return Response{"\"OK\""};
        });
    }

    void _bindEndpoint(const std::string& method,
                       rockets::jsonrpc::VoidCallback action)
    {
        _jsonrpcServer->connect(method, [&, action](
                                            rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);
            action();
            return Response{"\"OK\""};
        });
    }

    void _bindModelEndpoint(
        const std::string& method, const std::string& key,
        std::function<bool(std::string, ModelDescriptorPtr)> action)
    {
        _jsonrpcServer->bind(method, [&, key, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            using namespace rapidjson;
            Document document;
            document.Parse(request.message.c_str());

            if (!document.HasMember("id") || !document.HasMember(key.c_str()))
                return Response::invalidParams();

            const auto modelID = document["id"].GetInt();
            auto model = _engine.getScene().getModel(modelID);
            if (!model)
            {
                return Response{
                    Response::Error{"Model not found", MODEL_NOT_FOUND}};
            }

            // get the actual property object from the model message (e.g.
            // transfer function) and pass it as a JSON string to the provided
            // action to consume it.
            Document propertyDoc;
            propertyDoc.SetObject() = document[key.c_str()].GetObject();
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            propertyDoc.Accept(writer);

            if (!action(buffer.GetString(), model))
                return Response::invalidParams();

            _engine.triggerRender();
            _rebroadcast(method, request.message, {request.clientID});
            return Response{to_json(true)};
        });
    }

    template <class T>
    void _handleGET(const std::string& endpoint, T& obj,
                    const int64_t throttleTime = DEFAULT_THROTTLE)
    {
        using namespace rockets::http;

        _rocketsServer->handle(Method::GET, endpoint, [&obj](const Request&) {
            return make_ready_response(Code::OK, to_json(obj), JSON_TYPE);
        });

        _handleObjectSchema(endpoint, obj);

        const std::string rpcEndpoint = getRequestEndpointName(endpoint);

        _jsonrpcServer->bind(rpcEndpoint,
                             std::function<const T&()>(
                                 [&obj]() -> const T& { return obj; }));
        _handleSchema(rpcEndpoint,
                      buildJsonRpcSchemaRequestReturnOnly(
                          {rpcEndpoint, "Get the current state of " + endpoint},
                          obj));

        // Create new throttle for that endpoint
        _throttle[endpoint];

        obj.onModified([&, endpoint = getNotificationEndpointName(endpoint),
                        throttleTime](const auto& base) {
            auto& throttle = _throttle[endpoint];

            // throttle itself is not thread-safe, but we can get called
            // from different threads (c.f. async model load)
            std::lock_guard<std::mutex> lock(throttle.first);

            const auto& castedObj = static_cast<const T&>(base);
            const auto notify = [&rocketsServer = _rocketsServer,
                                 clientID = _currentClientID, endpoint,
                                 json = to_json(castedObj)] {
                if (rocketsServer->getConnectionCount() == 0)
                    return;
                try
                {
                    const auto& msg =
                        rockets::jsonrpc::makeNotification(endpoint, json);
                    if (clientID == NO_CURRENT_CLIENT)
                        rocketsServer->broadcastText(msg);
                    else
                        rocketsServer->broadcastText(msg, {clientID});
                }
                catch (const std::exception& e)
                {
                    BRAYNS_ERROR
                        << "Error broadcasting notification: " << e.what()
                        << std::endl;
                }
            };
            const auto delayedNotify = [&, notify] {
                this->_delayedNotify(notify);
            };

            // non-throttled, direct notify can happen directly if we are
            // not in the middle handling an incoming message; delayed
            // notify must be dispatched to the main thread
            if (_currentClientID == NO_CURRENT_CLIENT)
                throttle.second(notify, delayedNotify, throttleTime);
            else
                throttle.second(delayedNotify, delayedNotify, throttleTime);
        });

        _objects.push_back(&obj);
    }

    template <class T>
    void _handlePUT(const std::string& endpoint, T& obj)
    {
        _handlePUT(endpoint, obj, std::function<bool(const T&)>(),
                   std::function<void(T&)>());
    }

    template <class T, class PRE, class POST>
    void _handlePUT(const std::string& endpoint, T& obj, PRE preUpdateFunc,
                    POST postUpdateFunc)
    {
        using namespace rockets::http;
        _rocketsServer->handle(Method::PUT, endpoint,
                               [&obj, preUpdateFunc,
                                postUpdateFunc](const Request& req) {
                                   return make_ready_response(
                                       from_json(obj, req.body, preUpdateFunc,
                                                 postUpdateFunc)
                                           ? Code::OK
                                           : Code::BAD_REQUEST);
                               });

        _handleObjectSchema(endpoint, obj);

        const std::string rpcEndpoint = getNotificationEndpointName(endpoint);

        _bindEndpoint(rpcEndpoint, [&, rpcEndpoint, preUpdateFunc,
                                    postUpdateFunc](
                                       rockets::jsonrpc::Request request) {
            if (from_json(obj, request.message, preUpdateFunc, postUpdateFunc))
            {
                _engine.triggerRender();
                return rockets::jsonrpc::Response{to_json(true)};
            }
            return rockets::jsonrpc::Response::invalidParams();
        });
        const RpcParameterDescription desc{rpcEndpoint,
                                           "Set the new state of " + endpoint,
                                           "param", endpoint};
        _handleSchema(rpcEndpoint,
                      buildJsonRpcSchemaRequest<T, bool>(desc, obj));
    }

    template <class T>
    void _handle(const std::string& endpoint, T& obj,
                 const int64_t throttleTime = DEFAULT_THROTTLE)
    {
        _handleGET(endpoint, obj, throttleTime);
        _handlePUT(endpoint, obj);
    }

    template <class P, class R>
    void _handleRPC(const RpcParameterDescription& desc,
                    std::function<R(P)> action)
    {
        _bindEndpoint<P, R>(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaRequest<P, R>(desc));
    }

    template <class R>
    void _handleRPC(const RpcDescription& desc, std::function<R()> action)
    {
        _bindEndpoint<R>(desc.methodName, action);
        _handleSchema(desc.methodName,
                      buildJsonRpcSchemaRequestReturnOnly<R>(desc));
    }

    template <class P>
    void _handleRPC(const RpcParameterDescription& desc,
                    std::function<void(P)> action)
    {
        _bindEndpoint<P>(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaNotify<P>(desc));
    }

    void _handleRPC(const RpcDescription& desc, std::function<void()> action)
    {
        _bindEndpoint(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaNotify(desc));
    }

    template <class P, class R>
    void _handleAsyncRPC(const RpcParameterDescription& desc,
                         std::function<rockets::jsonrpc::CancelRequestCallback(
                             P, uintptr_t, rockets::jsonrpc::AsyncResponse,
                             rockets::jsonrpc::ProgressUpdateCallback)>
                             action)
    {
        _jsonrpcServer->bindAsync<P>(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaRequest<P, R>(desc));
    }

    template <class P, class R>
    void _handleTask(
        const RpcParameterDescription& desc,
        std::function<std::shared_ptr<Task<R>>(P, uintptr_t)> createTask)
    {
        // define the action that is executed on every incoming request from the
        // client:
        // - create the task that shall be executed
        // - wire the result of task to the response callback from rockets
        // - setup progress reporting during the task execution using libuv
        // - wire the cancel request from rockets to the task

        auto action = [&, createTask](P params, auto clientID, auto respond,
                                      auto progressCb BRAYNS_UNUSED) {
            // transform task error to rockets error response
            auto errorCallback = [&, respond](const TaskRuntimeError& error) {
                const Response response(
                    Response::Error{error.what(), error.code, error.data});
                this->_delayedNotify(
                    [respond, response] { respond(response); });
            };

            try
            {
                // transform task result to rockets response
                auto readyCallback = [&, respond](const R& result) {
                    try
                    {
                        this->_delayedNotify(
                            [respond, result] { respond({to_json(result)}); });
                    }
                    catch (const std::runtime_error& e)
                    {
                        const Response response(
                            Response::Error{e.what(),
                                            TASK_RESULT_TO_JSON_ERROR});
                        this->_delayedNotify(
                            [respond, response] { respond(response); });
                    }
                };

                // create the task that shall be executed for this request
                auto task = createTask(std::move(params), clientID);

                std::function<void()> finishProgress = [task] {
                    task->progress.update("Done", 1.f);
                };

// setup periodic progress reporting if we have libuv running
#ifdef BRAYNS_USE_LIBUV
                if (uvw::Loop::getDefault()->alive())
                {
                    auto progressUpdate =
                        uvw::Loop::getDefault()->resource<uvw::TimerHandle>();

                    auto sendProgress = [progressCb,
                                         &progress = task->progress] {
                        progress.consume(progressCb);
                    };
                    progressUpdate->on<uvw::TimerEvent>(
                        [sendProgress](const auto&, auto&) { sendProgress(); });

                    finishProgress = [task, progressUpdate, sendProgress] {
                        task->progress.update("Done", 1.f);
                        sendProgress();
                        progressUpdate->stop();
                        progressUpdate->close();
                    };

                    using ms = std::chrono::milliseconds;
                    progressUpdate->start(ms(0), ms(SLOW_THROTTLE));
                }
#endif

                // setup the continuation task that handles the result or error
                // of the task to handle the responses to rockets accordingly.
                auto responseTask =
                    std::make_shared<async::task<void>>(task->get().then(
                        [&, readyCallback, errorCallback, task,
                         finishProgress](typename Task<R>::Type result) {
                            finishProgress();

                            try
                            {
                                readyCallback(result.get());
                            }
                            catch (const TaskRuntimeError& e)
                            {
                                errorCallback(e);
                            }
                            catch (const std::exception& e)
                            {
                                errorCallback({e.what()});
                            }
                            catch (const async::task_canceled&)
                            {
                                task->finishCancel();
                            }

#ifdef BRAYNS_USE_LIBUV
                            if (_processDelayedNotifies)
                                _processDelayedNotifies->send();
#endif

                            std::lock_guard<std::mutex> lock(_tasksMutex);
                            _tasks.erase(task);
                            _binaryRequests.removeTask(task);
                        }));

                std::lock_guard<std::mutex> lock(_tasksMutex);
                _tasks.emplace(task, responseTask);

                // forward the cancel request from rockets to the task
                auto cancel = [task, responseTask](auto done) {
                    task->cancel(done);
                };

                task->schedule();

                return rockets::jsonrpc::CancelRequestCallback(cancel);
            }
            // respond errors during the setup of the task
            catch (const TaskRuntimeError& e)
            {
                errorCallback(e);
            }
            catch (const std::exception& e)
            {
                errorCallback({e.what()});
            }
            return rockets::jsonrpc::CancelRequestCallback();
        };
        _handleAsyncRPC<P, R>(desc, action);
    }

    template <class T>
    void _handleObjectSchema(const std::string& endpoint)
    {
        _handleSchema(endpoint,
                      buildJsonSchema<T>(hyphenatedToCamelCase(endpoint)));
    }

    template <class T>
    void _handleObjectSchema(const std::string& endpoint, T& obj)
    {
        _handleSchema(endpoint,
                      buildJsonSchema(obj, hyphenatedToCamelCase(endpoint)));
    }

    void _handleSchema(const std::string& endpoint, const std::string& schema)
    {
        using namespace rockets::http;
        _rocketsServer->handle(Method::GET, endpoint + "/schema",
                               [schema](const Request&) {
                                   return make_ready_response(Code::OK, schema,
                                                              JSON_TYPE);
                               });

        _schemas[endpoint] = schema;
    }

    void _registerEndpoints()
    {
        _handleAnimationParams();
        _handleCamera();
        _handleImageJPEG();
        _handleTriggerImageStream();
        _handleSetImageStreamingMode();
        _handleRenderer();
        _handleVersion();

        _handle(ENDPOINT_APP_PARAMS,
                _parametersManager.getApplicationParameters());
        _handle(ENDPOINT_VOLUME_PARAMS,
                _parametersManager.getVolumeParameters());

        // following endpoints need a valid engine
        const bool disableBroadcasting =
            std::getenv("ROCKETS_DISABLE_SCENE_BROADCASTING") != nullptr;
        if (disableBroadcasting)
            BRAYNS_WARN << "Scene broadcasting has been disabled" << std::endl;
        else
            _handle(ENDPOINT_SCENE, _engine.getScene());

        _handleGET(ENDPOINT_STATISTICS, _engine.getStatistics(), SLOW_THROTTLE);

        _handleSchemaRPC();

        _handleInspect();
        _handleQuit();
        _handleExitLater();
        _handleResetCamera();
        _handleSnapshot();

        _handleRequestModelUpload();
        _handleChunk();

        _handleSetEnvironmentMap();
        _handleGetEnvironmentMap();

        _handleSetVideostream();
        _handleGetVideostream();

        _handleAddModel();
        _handleRemoveModel();
        _handleUpdateModel();
        _handleSetModelProperties();
        _handleGetModelProperties();
        _handleModelPropertiesSchema();

        _handleSetModelTransferFunction();
        _handleGetModelTransferFunction();

        _handleAddClipPlane();
        _handleGetClipPlanes();
        _handleUpdateClipPlane();
        _handleRemoveClipPlanes();

        _handleGetInstances();
        _handleUpdateInstance();

        _handleGetLoaders();
        _handleLoadersSchema();
        _handlePropertyObject(_engine.getCamera(), ENDPOINT_CAMERA_PARAMS,
                              "camera");
        _handlePropertyObject(_engine.getRenderer(), ENDPOINT_RENDERER_PARAMS,
                              "renderer");
        _handleGetLights();
        _handleAddLight();
        _handleRemoveLights();
        _handleClearLights();

        _handleFsExists();
        _handleFsGetContent();
        _handleFsGetRoot();
        _handleFsGetListDir();

        _endpointsRegistered = true;
    }

    void _handleImageJPEG()
    {
        _jsonrpcServer->bind(METHOD_IMAGE_JPEG,
                             std::function<ImageGenerator::ImageBase64()>([&] {
                                 return _imageGenerator.createImage(
                                     _engine.getFrameBuffer(), "jpg",
                                     _parametersManager
                                         .getApplicationParameters()
                                         .getJpegCompression());
                             }));
        _handleSchema(
            METHOD_IMAGE_JPEG,
            buildJsonRpcSchemaRequestReturnOnly<ImageGenerator::ImageBase64>(
                {METHOD_IMAGE_JPEG,
                 "Get the current state of " + METHOD_IMAGE_JPEG}));
    }

    void _broadcastImageJpeg()
    {
        auto& frameBuffer = _engine.getFrameBuffer();
        if (frameBuffer.getFrameBufferFormat() == FrameBufferFormat::none ||
            !frameBuffer.isModified())
        {
            return;
        }

        const auto& params = _parametersManager.getApplicationParameters();
        const auto fps = params.getImageStreamFPS();
        if (fps == 0)
            return;

        const auto elapsed = _timer.elapsed() + _leftover;
        const auto duration = 1.0 / fps;
        if (elapsed < duration)
            return;

        _leftover = elapsed - duration;
        for (; _leftover > duration;)
            _leftover -= duration;
        _timer.start();

        const auto image =
            _imageGenerator.createJPEG(frameBuffer,
                                       params.getJpegCompression());
        if (image.size > 0)
            _rocketsServer->broadcastBinary((const char*)image.data.get(),
                                            image.size);
    }

    void _broadcastControlledImageJpeg()
    {
        if (!_controlledStreamingFlag.load())
        {
            return;
        }

        auto& frameBuffer = _engine.getFrameBuffer();
        if (frameBuffer.getFrameBufferFormat() == FrameBufferFormat::none ||
            !frameBuffer.isModified())
        {
            return;
        }

        _controlledStreamingFlag = false;
        const auto& params = _parametersManager.getApplicationParameters();

        const auto image =
            _imageGenerator.createJPEG(frameBuffer,
                                       params.getJpegCompression());
        if (image.size > 0)
            _rocketsServer->broadcastBinary((const char*)image.data.get(),
                                            image.size);
    }

#ifdef BRAYNS_USE_FFMPEG
    void _broadcastVideo()
    {
        if (!_videoParams.enabled)
        {
            _encoder.reset();
            if (_videoUpdatedResponse)
                _videoUpdatedResponse();
            _videoUpdatedResponse = nullptr;
            return;
        }

        const auto& params = _parametersManager.getApplicationParameters();
        const auto fps = params.getImageStreamFPS();
        if (fps == 0)
            return;

        if (_encoder && _encoder->kbps != _videoParams.kbps)
            _encoder.reset();

        auto& frameBuffer = _engine.getFrameBuffer();
        if (!_encoder)
        {
            int width = frameBuffer.getFrameSize().x;
            if (width % 2 != 0)
                width += 1;
            int height = frameBuffer.getFrameSize().y;
            if (height % 2 != 0)
                height += 1;

            _encoder =
                std::make_unique<Encoder>(width, height, fps, _videoParams.kbps,
                                          [&rs = _rocketsServer](auto a,
                                                                 auto b) {
                                              rs->broadcastBinary(a, b);
                                          });
        }

        if (_videoUpdatedResponse)
            _videoUpdatedResponse();
        _videoUpdatedResponse = nullptr;

        if (frameBuffer.getFrameBufferFormat() == FrameBufferFormat::none ||
            !frameBuffer.isModified())
        {
            return;
        }

        _encoder->encode(frameBuffer);
    }
#endif

    void _handleVersion()
    {
        static brayns::Version version;
        using namespace rockets::http;
        _rocketsServer->handleGET(ENDPOINT_VERSION, version);
        _rocketsServer->handle(
            Method::GET, ENDPOINT_VERSION + "/schema", [&](const Request&) {
                return make_ready_response(Code::OK, version.getSchema(),
                                           JSON_TYPE);
            });

        _jsonrpcServer->bind(
            getRequestEndpointName(ENDPOINT_VERSION),
            (std::function<brayns::Version()>)[] { return brayns::Version(); });

        _handleSchema(ENDPOINT_VERSION, version.getSchema());
    }

    void _handleAnimationParams()
    {
        auto& animParams = _parametersManager.getAnimationParameters();
        auto preUpdate = [](const AnimationParameters& obj) {
            return obj.getDelta() != 0;
        };

        _handleGET(ENDPOINT_ANIMATION_PARAMS, animParams, INTERACTIVE_THROTTLE);
        _handlePUT(ENDPOINT_ANIMATION_PARAMS, animParams, preUpdate,
                   std::function<void(AnimationParameters&)>());
    }

    void _handleCamera()
    {
        auto& camera = _engine.getCamera();
        auto preUpdate = [types = camera.getTypes()](const Camera& obj) {
            if (obj.getCurrentType().empty())
                return true;
            return std::find(types.begin(), types.end(),
                             obj.getCurrentType()) != types.end();
        };
        _handleGET(ENDPOINT_CAMERA, camera);
        _handlePUT(ENDPOINT_CAMERA, camera, preUpdate,
                   std::function<void(Camera&)>());
    }

    void _handleRenderer()
    {
        auto& params = _parametersManager.getRenderingParameters();
        auto preUpdate = [&params](const auto& rp) {
            return std::find(params.getRenderers().begin(),
                             params.getRenderers().end(),
                             rp.getCurrentRenderer()) !=
                   params.getRenderers().end();
        };
        auto postUpdate = [&renderer = _engine.getRenderer()](auto& rp) {
            renderer.setCurrentType(rp.getCurrentRenderer());
        };
        _handleGET(ENDPOINT_RENDERER, params);
        _handlePUT(ENDPOINT_RENDERER, params, preUpdate, postUpdate);
    }

    void _handleSchemaRPC()
    {
        const RpcParameterDescription desc{
            METHOD_SCHEMA, "Get the schema of the given endpoint",
            Execution::sync, "endpoint",
            "name of the endpoint to get its schema"};

        _jsonrpcServer->bind(METHOD_SCHEMA, [&schemas = _schemas](
                                                const auto& request) {
            SchemaParam param;
            if (::from_json(param, request.message))
            {
                if (schemas.count(param.endpoint) == 0)
                    return Response{
                        Response::Error{"Endpoint not found",
                                        SCHEMA_RPC_ENDPOINT_NOT_FOUND}};

                auto schema = schemas[param.endpoint];
                return Response{std::move(schema)};
            }
            return Response::invalidParams();
        });

        _handleSchema(METHOD_SCHEMA,
                      buildJsonRpcSchemaRequest<SchemaParam, std::string>(
                          desc));
    }

    void _handleInspect()
    {
        using Position = std::array<double, 2>;
        const RpcParameterDescription desc{
            METHOD_INSPECT, "Inspect the scene at x-y position",
            Execution::sync, "position",
            "x-y position in normalized coordinates"};
        _handleRPC<Position, Renderer::PickResult>(
            desc, [&engine = _engine](const auto& position) {
                return engine.getRenderer().pick(
                    {float(position[0]), float(position[1])});
            });
    }

    void _handleQuit()
    {
        _handleRPC({METHOD_QUIT, "Quit the application"}, [&engine = _engine] {
            engine.setKeepRunning(false);
            engine.triggerRender();
        });
    }

    void _handleExitLater()
    {
        _handleRPC<ExitLaterSchedule>(
            {METHOD_EXIT_LATER,
             "Schedules Brayns to shutdown after a given amount of minutes",
             "minutes", "Number of minutes after which Brayns will shut down"},
            [&](const ExitLaterSchedule& schedule) {
                std::lock_guard<std::mutex> lock(_scheduleMutex);
                if (schedule.minutes > 0)
                {
                    if (_scheduledShutdownActive)
                    {
                        _cancelScheduledShutdown = true;
                        _monitor.notify_all();
                        _shutDownWorker->join();
                        _shutDownWorker.reset();
                        _cancelScheduledShutdown = false;
                        _scheduledShutdownActive = false;
                    }

                    _scheduledShutdownActive = true;
                    const uint32_t mins = schedule.minutes;
                    _shutDownWorker = std::unique_ptr<std::thread>(
                        new std::thread([&, mins]() {
                            std::chrono::milliseconds timeToWait(mins * 60000);
                            std::unique_lock<std::mutex> threadLock(_waitLock);
                            _monitor.wait_for(threadLock, timeToWait);
                            if (!_cancelScheduledShutdown)
                            {
                                _engine.setKeepRunning(false);
                                _engine.triggerRender();
                            }
                        }));
                }
            });
    }

    void _handleResetCamera()
    {
        _handleRPC({METHOD_RESET_CAMERA,
                    "Resets the camera to its initial values"},
                   [&engine = _engine] {
                       engine.getCamera().reset();
                       engine.triggerRender();
                   });
    }

    void _handleSnapshot()
    {
        const RpcParameterDescription desc{
            METHOD_SNAPSHOT, "Make a snapshot of the current view",
            Execution::async, "settings",
            "Snapshot settings for quality and size"};
        auto func = [&engine = _engine,
                     &imageGenerator = _imageGenerator](auto&& params,
                                                        const auto) {
            using SnapshotTask = DeferredTask<ImageGenerator::ImageBase64>;
            return std::make_shared<SnapshotTask>(
                SnapshotFunctor{engine, std::move(params), imageGenerator});
        };
        _handleTask<SnapshotParams, ImageGenerator::ImageBase64>(desc, func);
    }

    void _handleTriggerImageStream()
    {
        _handleRPC({METHOD_TRIGGER_JPEG_STREAM,
                    "Triggers the engine to stream a frame to the clients"},
                   [&] { _triggerControlledStreaming(); });
    }

    void _handleSetImageStreamingMode()
    {
        _handleRPC<ImageStreamingMethod>(
            {METHOD_SET_STREAMING_METHOD,
             "Set the image streaming method between automatic or "
             "controlled",
             "type", "Streaming type, either \"stream\" or \"quanta\""},
            [&](const ImageStreamingMethod& method) {
                if (method.type == "quanta")
                {
                    _useControlledStream = true;
                    _controlledStreamingFlag = false;
                }
                else
                    _useControlledStream = false;
            });
    }

    void _handleRequestModelUpload()
    {
        const RpcParameterDescription desc{
            METHOD_REQUEST_MODEL_UPLOAD,
            "Request upload of blob to trigger adding of model after blob has "
            "been received; returns model descriptor on success",
            Execution::async, "param",
            "size, type, name, transformation, etc."};

        _handleTask<BinaryParam, ModelDescriptorPtr>(
            desc, std::bind(&BinaryRequests::createTask,
                            std::ref(_binaryRequests), std::placeholders::_1,
                            std::placeholders::_2, std::ref(_engine)));
    }

    void _handleChunk()
    {
        const RpcParameterDescription desc{
            METHOD_CHUNK,
            "Indicate sending of a binary chunk after this message", "chunk",
            "object with an ID of the chunk"};

        _handleRPC<Chunk>(desc, [&req = _binaryRequests](const auto& chunk) {
            req.setNextChunkID(chunk.id);
        });
    }

    void _handleSetModelTransferFunction()
    {
        const RpcParameterDescription desc{
            METHOD_SET_MODEL_TRANSFER_FUNCTION,
            "Set the transfer function of the given model", "param",
            "model ID and the new transfer function"};

        _bindModelEndpoint(METHOD_SET_MODEL_TRANSFER_FUNCTION,
                           "transfer_function",
                           [&](const std::string& json,
                               ModelDescriptorPtr model) {
                               auto& tf =
                                   model->getModel().getTransferFunction();
                               if (!::from_json(tf, json))
                                   return false;

                               tf.markModified();
                               return true;
                           });

        _handleSchema(METHOD_SET_MODEL_TRANSFER_FUNCTION,
                      buildJsonRpcSchemaRequest<ModelTransferFunction, bool>(
                          desc));
    }

    void _handleGetModelTransferFunction()
    {
        const RpcParameterDescription desc{
            METHOD_GET_MODEL_TRANSFER_FUNCTION,
            "Get the transfer function of the given model", "id",
            "the model ID"};

        _jsonrpcServer->bind<ObjectID, TransferFunction>(
            desc.methodName, [&engine = _engine](const ObjectID& id) {
                auto model = engine.getScene().getModel(id.id);
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);
                return model->getModel().getTransferFunction();
            });

        _handleSchema(METHOD_GET_MODEL_TRANSFER_FUNCTION,
                      buildJsonRpcSchemaRequest<ObjectID, TransferFunction>(
                          desc));
    }

    void _handleAddClipPlane()
    {
        const RpcParameterDescription desc{
            METHOD_ADD_CLIP_PLANE,
            "Add a clip plane; returns the clip plane descriptor", "plane",
            "An array of 4 floats"};

        _handleRPC<Plane, ClipPlanePtr>(desc, [&](const Plane& plane) {
            auto& scene = _engine.getScene();
            auto clipPlane = scene.getClipPlane(scene.addClipPlane(plane));
            _rebroadcast(METHOD_UPDATE_CLIP_PLANE, to_json(clipPlane),
                         {_currentClientID});
            return clipPlane;
        });
    }

    void _handleGetClipPlanes()
    {
        _handleRPC<ClipPlanes>({METHOD_GET_CLIP_PLANES, "Get all clip planes"},
                               [&engine = _engine]() {
                                   auto& scene = engine.getScene();
                                   return scene.getClipPlanes();
                               });
    }

    void _handleUpdateClipPlane()
    {
        const RpcParameterDescription desc{
            METHOD_UPDATE_CLIP_PLANE,
            "Update a clip plane with the given coefficients", "clip_plane",
            "Plane id and equation"};
        _handleRPC<ClipPlane, bool>(desc, [&](const ClipPlane& newPlane) {
            auto& scene = _engine.getScene();
            if (auto plane = scene.getClipPlane(newPlane.getID()))
            {
                plane->setPlane(newPlane.getPlane());
                _rebroadcast(METHOD_UPDATE_CLIP_PLANE, to_json(newPlane),
                             {_currentClientID});
                _engine.triggerRender();
                return true;
            }
            return false;
        });
    }

    void _handleRemoveClipPlanes()
    {
        const RpcParameterDescription desc{
            METHOD_REMOVE_CLIP_PLANES,
            "Remove clip planes from the scene given their gids", "ids",
            "Array of clip planes IDs"};
        _handleRPC<size_ts, bool>(desc, [&](const size_ts& ids) {
            for (const auto id : ids)
                _engine.getScene().removeClipPlane(id);
            _rebroadcast(METHOD_REMOVE_CLIP_PLANES, to_json(ids),
                         {_currentClientID});
            _engine.triggerRender();
            return true;
        });
    }

    void _handleGetLights()
    {
        const RpcDescription desc{METHOD_GET_LIGHTS, "get all lights"};
        _bindEndpoint(METHOD_GET_LIGHTS, [&engine = _engine](
                                             const rockets::jsonrpc::
                                                 Request& /*request*/) {
            const auto& lights =
                engine.getScene().getLightManager().getLights();

            std::vector<std::string> jsonStrings;

            for (const auto& kv : lights)
            {
                RPCLight rpcLight;
                rpcLight.id = kv.first;
                auto baseLight = kv.second;

                switch (baseLight->_type)
                {
                case LightType::DIRECTIONAL:
                {
                    rpcLight.type = "directional";
                    const auto light =
                        static_cast<DirectionalLight*>(baseLight.get());
                    rpcLight.properties.setProperty(
                        {"direction", toArray<3, double>(light->_direction)});
                    rpcLight.properties.setProperty(
                        {"angularDiameter", light->_angularDiameter});
                    break;
                }
                case LightType::SPHERE:
                {
                    rpcLight.type = "sphere";
                    const auto light =
                        static_cast<SphereLight*>(baseLight.get());
                    rpcLight.properties.setProperty(
                        {"position", toArray<3, double>(light->_position)});
                    rpcLight.properties.setProperty({"radius", light->_radius});
                    break;
                }
                case LightType::QUAD:
                {
                    rpcLight.type = "quad";
                    const auto light = static_cast<QuadLight*>(baseLight.get());
                    rpcLight.properties.setProperty(
                        {"position", toArray<3, double>(light->_position)});
                    rpcLight.properties.setProperty(
                        {"edge1", toArray<3, double>(light->_edge1)});
                    rpcLight.properties.setProperty(
                        {"edge2", toArray<3, double>(light->_edge2)});
                    break;
                }
                case LightType::SPOTLIGHT:
                {
                    rpcLight.type = "spotlight";
                    const auto light = static_cast<SpotLight*>(baseLight.get());
                    rpcLight.properties.setProperty(
                        {"position", toArray<3, double>(light->_position)});
                    rpcLight.properties.setProperty(
                        {"direction", toArray<3, double>(light->_direction)});
                    rpcLight.properties.setProperty(
                        {"openingAngle", light->_openingAngle});
                    rpcLight.properties.setProperty(
                        {"penumbraAngle", light->_penumbraAngle});
                    rpcLight.properties.setProperty({"radius", light->_radius});
                    break;
                }
                case LightType::AMBIENT:
                {
                    rpcLight.type = "ambient";
                    break;
                }
                }

                rpcLight.properties.setProperty(
                    {"color", toArray<3, double>(baseLight->_color)});
                rpcLight.properties.setProperty(
                    {"intensity", baseLight->_intensity});
                rpcLight.properties.setProperty(
                    {"isVisible", baseLight->_isVisible});

                jsonStrings.emplace_back(to_json(rpcLight));
            }
            return Response{"[" + string_utils::join(jsonStrings, ",") + "]"};
        });

        _handleSchema(
            METHOD_GET_LIGHTS,
            buildJsonRpcSchemaRequestReturnOnly<std::vector<RPCLight>>(desc));
    }

    void _handleAddLight()
    {
        _handleRPC<SpotLight, int>({METHOD_ADD_LIGHT_SPOT,
                                    "Add a spotlight, returns id", "light",
                                    "The light and its properties"},
                                   [&engine = _engine](const SpotLight& l) {
                                       LightManager& lightManager =
                                           engine.getScene().getLightManager();
                                       auto light =
                                           std::make_shared<SpotLight>(l);
                                       light->_type = LightType::SPOTLIGHT;

                                       const auto id =
                                           lightManager.addLight(light);
                                       engine.triggerRender();
                                       return id;
                                   });

        _handleRPC<DirectionalLight, int>(
            {METHOD_ADD_LIGHT_DIRECTIONAL, "Add a directional light", "light",
             "The light and its properties"},
            [&engine = _engine](const DirectionalLight& l) {
                LightManager& lightManager =
                    engine.getScene().getLightManager();
                auto light = std::make_shared<DirectionalLight>(l);
                light->_type = LightType::DIRECTIONAL;

                const auto id = lightManager.addLight(light);
                engine.triggerRender();
                return id;
            });

        _handleRPC<QuadLight, int>({METHOD_ADD_LIGHT_QUAD, "Add a quad light",
                                    "light", "The light and its properties"},
                                   [&engine = _engine](const QuadLight& l) {
                                       LightManager& lightManager =
                                           engine.getScene().getLightManager();
                                       auto light =
                                           std::make_shared<QuadLight>(l);
                                       light->_type = LightType::QUAD;

                                       const auto id =
                                           lightManager.addLight(light);
                                       engine.triggerRender();
                                       return id;
                                   });

        _handleRPC<SphereLight, int>(
            {METHOD_ADD_LIGHT_SPHERE, "Add a sphere light", "light",
             "The light and its properties"},
            [&engine = _engine](const SphereLight& l) {
                LightManager& lightManager =
                    engine.getScene().getLightManager();
                auto light = std::make_shared<SphereLight>(l);
                light->_type = LightType::SPHERE;

                const auto id = lightManager.addLight(light);
                engine.triggerRender();
                return id;
            });

        _handleRPC<AmbientLight, int>(
            {METHOD_ADD_LIGHT_AMBIENT, "Add an ambient light", "light",
             "The light and its properties"},
            [&engine = _engine](const AmbientLight& l) {
                LightManager& lightManager =
                    engine.getScene().getLightManager();
                auto light = std::make_shared<AmbientLight>(l);
                light->_type = LightType::AMBIENT;

                const auto id = lightManager.addLight(light);
                engine.triggerRender();
                return id;
            });
    }

    void _handleRemoveLights()
    {
        const RpcParameterDescription desc{METHOD_REMOVE_LIGHTS,
                                           "Remove light given their IDs",
                                           "ids", "Array of light IDs"};
        _handleRPC<size_ts, bool>(desc, [&engine =
                                             _engine](const size_ts& ids) {
            auto& lightManager = engine.getScene().getLightManager();
            for (const auto id : ids)
                lightManager.removeLight(id);
            engine.triggerRender();
            return true;
        });
    }

    void _handleClearLights()
    {
        const RpcDescription desc{METHOD_CLEAR_LIGHTS,
                                  "Remove all lights in the scene"};
        _handleRPC(desc, [&engine = _engine]() {
            auto& lightManager = engine.getScene().getLightManager();
            lightManager.clearLights();
            engine.triggerRender();
        });
    }

    FileStats _getFileStats(const std::string& path)
    {
        FileStats ft;
        ft.type = "none";
        ft.error = 0;
        ft.sizeBytes = 0;

        // Make sure the requested file is within the sandbox path
        // TODO: Unhardcode sandbox path and add as braynsService input param
        const std::string& sandboxPath = _engine.getParametersManager()
                                             .getApplicationParameters()
                                             .getSandboxPath();
        size_t pos = path.find(sandboxPath);
        if (pos != 0)
        {
            ft.error = 1;
            ft.message = "Path falls outside the sandbox: " + sandboxPath;
            return ft;
        }

        if (path.find("../") == 0 || path.find("/../") != std::string::npos ||
            path.rfind("/..") == 4)
        {
            ft.error = 3;
            ft.message = "Illegal path detected";
            return ft;
        }

        struct stat s;
        int err = stat(path.c_str(), &s);
        if (err == 0)
        {
            // Path is a regular file
            if (s.st_mode & S_IFREG)
            {
                // Test permissions with fopen
                FILE* permissionTest = fopen(path.c_str(), "r");
                int openError = errno;
                if (permissionTest != nullptr)
                    fclose(permissionTest);
                if (openError == EACCES)
                {
                    ft.error = 2;
                    ft.message = "Permission for " + path + " denied";
                    return ft;
                }

                // If we reached this point, file is accessible
                ft.type = "file";
                ft.sizeBytes = s.st_size;
            }
            // Path is a folder
            else if (s.st_mode & S_IFDIR)
            {
                // Test permissions with access
                if (access(path.c_str(), X_OK) < 0)
                {
                    if (errno == EACCES)
                    {
                        ft.error = 2;
                        ft.message = "Permission for " + path + " denied";
                        return ft;
                    }
                    else
                    {
                        ft.error = 3;
                        ft.message = "Unknown error";
                        return ft;
                    }
                }

                // If we reached this point, folder is accessible
                ft.type = "directory";
            }
            else
            {
                ft.message = "Unknown type of element";
            }
        }
        else if (err > 0)
        {
            if (err == EACCES)
            {
                ft.error = 2;
                ft.message = "Permission for " + path + " denied";
            }
            else
            {
                ft.error = 3;
                ft.message = "Unknown error";
            }
        }

        return ft;
    }

    void _handleFsExists()
    {
        const RpcParameterDescription desc{
            METHOD_FS_EXISTS,
            "Return the type of filer (file or folder) if a given path exists, "
            "or none if it does not exists",
            "path", "Absolute path to the filer to check"};
        _handleRPC<InputPath, FileType>(desc, [&](const auto& inputPath) {
            this->_rebroadcast(METHOD_FS_EXISTS, to_json(inputPath),
                               {_currentClientID});
            FileStats fs = this->_getFileStats(inputPath.path);
            FileType ft;
            ft.type = fs.type;
            ft.error = fs.error;
            ft.message = fs.message;
            return ft;
        });
    }

    void _handleFsGetContent()
    {
        const RpcParameterDescription desc{
            METHOD_FS_GET_CONTENT,
            "Return the content of a file if possible, or an error otherwise",
            "path", "Absolute path to the file"};
        _handleRPC<InputPath, FileContent>(desc, [&](const auto& inputPath) {
            this->_rebroadcast(METHOD_FS_GET_CONTENT, to_json(inputPath),
                               {_currentClientID});
            // Stat the requested file
            FileStats ft = this->_getFileStats(inputPath.path);
            FileContent fc;

            fc.error = ft.error;
            fc.message = ft.message;

            // Continue only if its accessible and its a file
            if (fc.error == 0 && ft.type == "file")
            {
                std::ifstream file(inputPath.path);
                if (file)
                {
                    // Read file
                    file.seekg(0, file.end);
                    long len = file.tellg();
                    file.seekg(0, file.beg);
                    std::vector<char> buffer(static_cast<unsigned long>(len));
                    file.read(&buffer[0], len);
                    file.close();
                    fc.content = std::string(buffer.begin(), buffer.end());
                }
                else
                {
                    fc.error = 3;
                    fc.message =
                        "An unknown error occurred when reading the file " +
                        inputPath.path;
                }
            }

            return fc;
        });
    }

    void _handleFsGetRoot()
    {
        const RpcDescription desc{METHOD_FS_GET_ROOT,
                                  "Return the root path of the current "
                                  "execution environment (sandbox)"};
        _bindEndpoint(METHOD_FS_GET_ROOT,
                      [&engine = _engine](const rockets::jsonrpc::Request&) {
                          FileRoot fr;
                          fr.root = engine.getParametersManager()
                                        .getApplicationParameters()
                                        .getSandboxPath();
                          return Response{to_json(fr)};
                      });

        _handleSchema(METHOD_GET_LIGHTS,
                      buildJsonRpcSchemaRequestReturnOnly<FileRoot>(desc));
    }

    void _handleFsGetListDir()
    {
        const RpcParameterDescription desc{
            METHOD_FS_LIST_DIR,
            "Return the content of a file if possible, or an error otherwise",
            "path", "Absolute path to the file"};
        _handleRPC<InputPath, DirectoryFileList>(desc, [&](const auto&
                                                               inputPath) {
            this->_rebroadcast(METHOD_FS_LIST_DIR, to_json(inputPath),
                               {_currentClientID});

            // Stat the requested path
            FileStats ft = this->_getFileStats(inputPath.path);

            DirectoryFileList dfl;
            dfl.error = ft.error;
            dfl.message = ft.message;

            // Continue only if its a directory and is accessible
            const bool isDirectory = ft.type == "directory";
            if (ft.error == 0 && isDirectory)
            {
                DIR* dir;
                struct dirent* ent;
                if ((dir = opendir(inputPath.path.c_str())) != nullptr)
                {
                    // Iterate over each entry of the directory
                    std::string slash =
                        inputPath.path[inputPath.path.size() - 1] == '/' ? ""
                                                                         : "/";
                    while ((ent = readdir(dir)) != nullptr)
                    {
                        const std::string fileName(ent->d_name);

                        // Discard dots
                        if (fileName == "." || fileName == "..")
                            continue;

                        // Get file status to known if accessible, type, and
                        // size
                        const std::string filePath =
                            inputPath.path + slash + fileName;
                        FileStats fileStats = this->_getFileStats(filePath);

                        if (fileStats.error == 0)
                        {
                            if (fileStats.type == "directory")
                            {
                                dfl.dirs.push_back(fileName);
                            }
                            else if (fileStats.type == "file")
                            {
                                // Calc size in octets
                                size_t totalBits =
                                    static_cast<size_t>(fileStats.sizeBytes) *
                                    static_cast<size_t>(CHAR_BIT);
                                size_t totalOctets = totalBits / 8;
                                dfl.files.names.push_back(fileName);
                                dfl.files.sizes.push_back(totalOctets);
                            }
                        }
                    }
                    closedir(dir);
                }
                else
                {
                    dfl.error = 3;
                    dfl.message =
                        "Unknown error when reading contents of directory " +
                        inputPath.path;
                }
            }
            else if (!isDirectory && ft.error == 0)
            {
                dfl.error = 4;
                dfl.message =
                    "The path " + inputPath.path + " is not a directory";
            }

            return dfl;
        });
    }

    void _handleAddModel()
    {
        const RpcParameterDescription desc{
            METHOD_ADD_MODEL,
            "Add model from remote path; returns model descriptor on success",
            Execution::async, "model_param",
            "Model parameters including name, path, transformation, etc."};

        auto func = [&](const ModelParams& modelParams, const auto) {
            return std::make_shared<AddModelTask>(modelParams, _engine);
        };
        _handleTask<ModelParams, ModelDescriptorPtr>(desc, func);
    }

    void _handleRemoveModel()
    {
        const RpcParameterDescription desc{
            METHOD_REMOVE_MODEL,
            "Remove the model(s) with the given ID(s) from the scene", "ids",
            "Array of model IDs"};
        _handleRPC<size_ts, bool>(desc,
                                  [&engine = _engine](const size_ts& ids) {
                                      for (const auto id : ids)
                                          engine.getScene().removeModel(id);
                                      engine.triggerRender();
                                      return true;
                                  });
    }

    void _handleUpdateModel()
    {
        _bindEndpoint(METHOD_UPDATE_MODEL,
                      [&engine =
                           _engine](const rockets::jsonrpc::Request& request) {
                          ModelDescriptor newDesc;
                          if (!::from_json(newDesc, request.message))
                              return Response::invalidParams();

                          auto& scene = engine.getScene();
                          if (auto model = scene.getModel(newDesc.getModelID()))
                          {
                              ::from_json(*model, request.message);
                              scene.markModified();
                              engine.triggerRender();
                              return Response{to_json(true)};
                          }
                          return Response{to_json(false)};
                      });
        const RpcParameterDescription desc{
            METHOD_UPDATE_MODEL, "Update the model with the given values",
            "model", "Model descriptor"};
        _handleSchema(METHOD_UPDATE_MODEL,
                      buildJsonRpcSchemaRequest<ModelDescriptor, bool>(desc));
    }

    void _handleGetModelProperties()
    {
        const RpcParameterDescription desc{
            METHOD_GET_MODEL_PROPERTIES,
            "Get the properties of the given model", "id", "the model ID"};

        _jsonrpcServer->bind<ObjectID, PropertyMap>(
            desc.methodName, [&engine = _engine](const ObjectID& id) {
                auto model = engine.getScene().getModel(id.id);
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);
                return model->getProperties();
            });

        _handleSchema(METHOD_GET_MODEL_PROPERTIES,
                      buildJsonRpcSchemaRequest<ObjectID, PropertyMap>(desc));
    }

    void _handleSetModelProperties()
    {
        const RpcParameterDescription desc{
            METHOD_SET_MODEL_PROPERTIES,
            "Set the properties of the given model", "param",
            "model ID and its properties"};

        _bindModelEndpoint(METHOD_SET_MODEL_PROPERTIES, "properties",
                           [&](const std::string& json,
                               ModelDescriptorPtr model) {
                               auto props = model->getProperties();
                               props.merge(jsonToPropertyMap(json));
                               model->setProperties(props);
                               return true;
                           });

        _handleSchema(METHOD_SET_MODEL_PROPERTIES,
                      buildJsonRpcSchemaRequest<ModelProperties, bool>(desc));
    }

    void _handleModelPropertiesSchema()
    {
        const RpcParameterDescription desc{
            METHOD_MODEL_PROPERTIES_SCHEMA,
            "Get the property schema of the model", "id",
            "ID of the model get its properties schema"};

        _jsonrpcServer->bind(
            METHOD_MODEL_PROPERTIES_SCHEMA,
            [&engine = _engine](const auto& request) {
                ObjectID modelID;
                if (::from_json(modelID, request.message))
                {
                    auto model = engine.getScene().getModel(modelID.id);
                    if (!model)
                        return Response{Response::Error{"Model not found",
                                                        MODEL_NOT_FOUND}};

                    return Response{buildJsonSchema(model->getProperties(),
                                                    "ModelProperties")};
                }
                return Response::invalidParams();
            });

        _handleSchema(METHOD_MODEL_PROPERTIES_SCHEMA,
                      buildJsonRpcSchemaRequest<ObjectID, std::string>(desc));
    }

    void _handleGetInstances()
    {
        const RpcParameterDescription desc{METHOD_GET_INSTANCES,
                                           "Get instances", "id, range",
                                           "Model id and result range"};
        _handleRPC<GetInstances, ModelInstances>(
            desc,
            [&engine = _engine](const GetInstances& param) -> ModelInstances {
                auto id = param.modelID;
                auto& scene = engine.getScene();
                auto model = scene.getModel(id);
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);

                const auto& instances = model->getInstances();
                const Vector2ui range{std::min(param.resultRange.x,
                                               unsigned(instances.size())),
                                      std::min(param.resultRange.y,
                                               unsigned(instances.size()))};
                return {instances.begin() + range.x,
                        instances.begin() + range.y};
            });
    }

    void _handleGetLoaders()
    {
        _handleRPC<std::vector<LoaderInfo>>(
            {METHOD_GET_LOADERS, "Get all loaders"}, [&] {
                auto& scene = _engine.getScene();
                return scene.getLoaderRegistry().getLoaderInfos();
            });
    }

    void _handleUpdateInstance()
    {
        _bindEndpoint(
            METHOD_UPDATE_INSTANCE,
            [&](const rockets::jsonrpc::Request& request) {
                ModelInstance newDesc;
                if (!::from_json(newDesc, request.message))
                    return Response::invalidParams();

                auto& scene = _engine.getScene();
                auto model = scene.getModel(newDesc.getModelID());
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);

                auto instance = model->getInstance(newDesc.getInstanceID());
                if (!instance)
                    throw rockets::jsonrpc::response_error("Instance not found",
                                                           INSTANCE_NOT_FOUND);

                ::from_json(*instance, request.message);
                model->getModel().markInstancesDirty();
                scene.markModified(false);

                _engine.triggerRender();
                _rebroadcast(METHOD_UPDATE_INSTANCE, request.message,
                             {request.clientID});

                return Response{to_json(true)};
            });
        const RpcParameterDescription desc{
            METHOD_UPDATE_INSTANCE, "Update the instance with the given values",
            "model_instance", "Model instance"};
        _handleSchema(METHOD_UPDATE_INSTANCE,
                      buildJsonRpcSchemaRequest<ModelInstance, bool>(desc));
    }

    void _handlePropertyObject(PropertyObject& object,
                               const std::string& endpoint,
                               const std::string objectName)
    {
        const auto requestEndpoint = getRequestEndpointName(endpoint);
        const auto notifyEndpoint = getNotificationEndpointName(endpoint);

        _jsonrpcServer->bind<PropertyMap>(requestEndpoint, [&object = object] {
            return object.getPropertyMap();
        });

        _bindEndpoint(notifyEndpoint, [&, notifyEndpoint](const auto& request) {
            object.updateProperties(jsonToPropertyMap(request.message));
            _engine.triggerRender();
            this->_rebroadcast(notifyEndpoint, request.message,
                               {request.clientID});
            return Response{to_json(true)};
        });

        std::vector<std::pair<std::string, PropertyMap>> props;
        for (const auto& type : object.getTypes())
            props.push_back(std::make_pair(type, object.getPropertyMap(type)));

        // get-<object>-params RPC schema
        _handleSchema(requestEndpoint,
                      buildJsonRpcSchemaRequestPropertyMaps(
                          {requestEndpoint,
                           "Get the params of the current " + objectName},
                          props));

        // set-<object>-params RPC schema
        const RpcParameterDescription desc{notifyEndpoint,
                                           "Set the params on the current " +
                                               objectName,
                                           "params",
                                           "new " + objectName + " params"};
        _handleSchema(notifyEndpoint,
                      buildJsonRpcSchemaNotifyPropertyMaps(desc, props));

        // <object>-params object schema
        _handleSchema(endpoint,
                      buildJsonSchema(props, hyphenatedToCamelCase(endpoint)));
    }

    void _handleLoadersSchema()
    {
        const RpcDescription desc{LOADERS_SCHEMA,
                                  "Get the schema for all loaders"};

        _bindEndpoint(
            LOADERS_SCHEMA, [&](const rockets::jsonrpc::Request& /*request*/) {
                const auto& loaderInfos =
                    _engine.getScene().getLoaderRegistry().getLoaderInfos();

                std::vector<std::pair<std::string, PropertyMap>> props;
                for (const LoaderInfo& li : loaderInfos)
                    props.emplace_back(li.name, li.properties);

                return Response{buildJsonSchema(props, "loaders")};
            });

        _handleSchema(
            LOADERS_SCHEMA,
            buildJsonRpcSchemaRequestReturnOnly<std::vector<PropertyMap>>(
                desc));
    }

    void _handleSetEnvironmentMap()
    {
        const RpcParameterDescription desc{METHOD_SET_ENVIRONMENT_MAP,
                                           "Set a environment map in the scene",
                                           "filename",
                                           "environment map texture file"};

        _handleRPC<EnvironmentMapParam, bool>(desc, [&](const auto& envMap) {
            this->_rebroadcast(METHOD_SET_ENVIRONMENT_MAP, to_json(envMap),
                               {_currentClientID});
            if (_engine.getScene().setEnvironmentMap(envMap.filename))
            {
                _engine.triggerRender();
                return true;
            }
            return false;
        });
    }

    void _handleGetEnvironmentMap()
    {
        const RpcDescription desc{METHOD_GET_ENVIRONMENT_MAP,
                                  "Get the environment map from the scene"};

        _handleRPC<EnvironmentMapParam>(desc, [&]() -> EnvironmentMapParam {
            return {_engine.getScene().getEnvironmentMap()};
        });
    }

    void _handleSetVideostream()
    {
        const RpcParameterDescription desc{METHOD_SET_VIDEOSTREAM,
                                           "Set the video streaming parameters",
                                           "params", "videostream parameters"};

        auto action = [&](const VideoStreamParam& params BRAYNS_UNUSED,
                          auto clientID BRAYNS_UNUSED, auto respond, auto) {
            if (!_parametersManager.getApplicationParameters()
                     .useVideoStreaming())
            {
                respond(
                    Response{Response::Error(VIDEOSTREAM_NOT_ENABLED_ERROR)});
                return rockets::jsonrpc::CancelRequestCallback();
            }

#ifdef BRAYNS_USE_FFMPEG
            this->_rebroadcast(METHOD_SET_VIDEOSTREAM, to_json(params),
                               {clientID});

            const bool changed = params != _videoParams;
            if (!changed)
            {
                respond(Response{to_json(false)});
                return rockets::jsonrpc::CancelRequestCallback();
            }

            _engine.triggerRender();
            _videoParams = params;
            _videoUpdatedResponse = [respond] {
                respond(Response{to_json(true)});
            };
            return rockets::jsonrpc::CancelRequestCallback();
#else
            respond(Response{Response::Error(VIDEOSTREAM_NOT_SUPPORTED_ERROR)});
            return rockets::jsonrpc::CancelRequestCallback();
#endif
        };
        _handleAsyncRPC<VideoStreamParam, bool>(desc, action);
    }

    void _handleGetVideostream()
    {
        const RpcDescription desc{METHOD_GET_VIDEOSTREAM,
                                  "Get the videostream parameters"};

        _handleRPC<VideoStreamParam>(desc, [&]() -> VideoStreamParam {
#ifdef BRAYNS_USE_FFMPEG
            if (!_parametersManager.getApplicationParameters()
                     .useVideoStreaming())
                throw rockets::jsonrpc::response_error(
                    VIDEOSTREAM_NOT_ENABLED_ERROR);
            return _videoParams;
#else
            throw rockets::jsonrpc::response_error(VIDEOSTREAM_NOT_SUPPORTED_ERROR);
#endif
        });
    }

    void _triggerControlledStreaming()
    {
        _controlledStreamingFlag = true;
        _engine.triggerRender();
    }

    Engine& _engine;

    std::unordered_map<std::string, std::pair<std::mutex, Throttle>> _throttle;
    std::vector<std::function<void()>> _delayedNotifies;
    std::mutex _delayedNotifiesMutex;
    static constexpr uintptr_t NO_CURRENT_CLIENT{0};
    uintptr_t _currentClientID{NO_CURRENT_CLIENT};

#ifdef BRAYNS_USE_LIBUV
    std::shared_ptr<uvw::AsyncHandle> _processDelayedNotifies;
#endif

    std::unordered_map<std::string, std::string> _schemas;

    ParametersManager& _parametersManager;

    std::unique_ptr<rockets::Server> _rocketsServer;
    using JsonRpcServer = rockets::jsonrpc::Server<rockets::Server>;
    std::unique_ptr<JsonRpcServer> _jsonrpcServer;

    bool _manualProcessing{true};

    ImageGenerator _imageGenerator;

    Timer _timer;
    float _leftover{0.f};

    std::map<TaskPtr, std::shared_ptr<async::task<void>>> _tasks;
    std::mutex _tasksMutex;

    BinaryRequests _binaryRequests;
    // need to delay those as we are initialized first, but other plugins might
    // alter the list of renderers for instance
    bool _endpointsRegistered{false};

    // Wether to use controlled stream (true = client request frames, false =
    // continous stream of frames)
    bool _useControlledStream{false};
    // Flag used to control the frame send when _useControlledStream = true
    std::atomic<bool> _controlledStreamingFlag{false};

    // Wether a scheduled shutdown is running at the momment
    bool _scheduledShutdownActive{false};
    // Flag to cancel current scheduled shutdown
    bool _cancelScheduledShutdown{false};
    // Worker in charge of shutdown
    std::unique_ptr<std::thread> _shutDownWorker;
    // Lock for safe schedule
    std::mutex _scheduleMutex;
    // Schedule mechanism
    std::mutex _waitLock;
    std::condition_variable _monitor;

    std::vector<BaseObject*> _objects;

#ifdef BRAYNS_USE_FFMPEG
    std::unique_ptr<Encoder> _encoder;
    VideoStreamParam _videoParams;
    std::function<void()> _videoUpdatedResponse;
#endif
};

RocketsPlugin::~RocketsPlugin()
{
    _api->setActionInterface(ActionInterfacePtr());
}

void RocketsPlugin::init()
{
    _impl = std::make_shared<Impl>(_api);
    _api->setActionInterface(_impl);
}

void RocketsPlugin::preRender()
{
    _impl->preRender();
}
void RocketsPlugin::postRender()
{
    _impl->postRender();
}
} // namespace brayns
