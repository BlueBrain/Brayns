/* Copyright 2015-2024 Blue Brain Project/EPFL
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/common/Blob.h>
#include <brayns/engine/Model.h>
#include <brayns/json/Json.h>
#include <brayns/json/JsonSchema.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/StringUtils.h>

#include <functional>

namespace brayns
{
/**
 * A class for providing progress feedback
 */
class LoaderProgress
{
public:
    /**
     * The callback for each progress update with the signature (message,
     * fraction of progress in 0..1 range)
     */
    using CallbackFn = std::function<void(const std::string&, float)>;

    LoaderProgress(CallbackFn callback)
        : _callback(std::move(callback))
    {
    }

    LoaderProgress() = default;
    ~LoaderProgress() = default;

    /**
     * Update the current progress of an operation and call the callback
     */
    void updateProgress(const std::string& message, const float fraction) const
    {
        if (_callback)
            _callback(message, fraction);
    }

    CallbackFn _callback;
};

/**
 * @brief Base class to allow template-parametrized loaders to define their
 *        input parameters at compile time
 */
class AbstractLoader
{
public:
    virtual ~AbstractLoader() = default;

    /**
     * @return The loaders supported file extensions
     */
    virtual std::vector<std::string> getSupportedExtensions() const = 0;

    /**
     * @brief Query the loader if it can load the given file. Compares the file
     * extension to the supported extensions (removing the dot, if any, and
     * making them lowercase). Can be overriden to perform mor explicit checks
     */
    virtual bool isSupported(const std::string& filename,
                             const std::string& extension) const
    {
        const auto extensions = getSupportedExtensions();
        const auto lcExtension = string_utils::toLowercase(
            extension[0] == '.' ? extension.substr(1) : extension);
        auto it =
            std::find_if(extensions.begin(), extensions.end(),
                         [&](const std::string& ext) {
                             return string_utils::toLowercase(
                                        ext[0] == '.' ? ext.substr(1) : ext) ==
                                    lcExtension;
                         });
        return it != extensions.end();
    };

    /**
     * @return The loader name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief returns the loader input parameter schema to be used on loader
     *        discovery
     */
    virtual const JsonSchema& getInputParametersSchema() const = 0;

    /**
     * @brief Loads a model/list of models from a blob of memory
     * @param blob the memory blob containing the data to be loaded
     * @param callback a callback to update the progress to the caller
     * @param params a brayns::JsonValue with the json payload of the input
     * parameters which will be deserialized into the selected loader's input
     * parameter type.
     * @return a std::vector with all the brayns::ModelDescriptor loaded from
     * the blob
     */
    virtual std::vector<ModelDescriptorPtr> loadFromBlob(
        Blob&& blob, const LoaderProgress& callback, const JsonValue& params,
        Scene& scene) const = 0;

    /**
     * @brief Loads a model/list of models from a path to a file
     * @param path path to the file on the current filesystem
     * @param callback a callback to update the progress to the caller
     * @param params a brayns::JsonValue with the json payload of the input
     * parameters which will be deserialized into the selected loader's input
     * parameter type.
     * @return a std::vector with all the brayns::ModelDescriptor loaded from
     * the file
     */
    virtual std::vector<ModelDescriptorPtr> loadFromFile(
        const std::string& path, const LoaderProgress& callback,
        const JsonValue& params, Scene& scene) const = 0;
};

/**
 * A base class for data loaders to unify loading data from blobs and files, and
 * provide progress feedback.
 */
template <typename T>
class Loader : public AbstractLoader
{
public:
    Loader()
        : _parameterSchema(Json::getSchema<T>())
    {
    }

    virtual ~Loader() = default;

    virtual const JsonSchema& getInputParametersSchema() const override
    {
        return _parameterSchema;
    }

    virtual std::vector<ModelDescriptorPtr> loadFromBlob(
        Blob&& blob, const LoaderProgress& callback, const JsonValue& params,
        Scene& scene) const override
    {
        const T inputParams = _parseParameters(params);
        return importFromBlob(std::move(blob), callback, inputParams, scene);
    }

    virtual std::vector<ModelDescriptorPtr> loadFromFile(
        const std::string& path, const LoaderProgress& callback,
        const JsonValue& params, Scene& scene) const override
    {
        const T inputParams = _parseParameters(params);
        return importFromFile(path, callback, inputParams, scene);
    }

    /**
     * Import the data from the blob and return the created model.
     *
     * @param blob the blob containing the data to import
     * @param callback Callback for loader progress
     * @param properties Properties used for loading
     * @return the model that has been created by the loader
     */
    virtual std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback, const T& properties,
        Scene& scene) const = 0;

    /**
     * Import the data from the given file and return the created model.
     *
     * @param filename the file containing the data to import
     * @param callback Callback for loader progress
     * @param properties Properties used for loading
     * @return the model that has been created by the loader
     */
    virtual std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& filename, const LoaderProgress& callback,
        const T& properties, Scene& scene) const = 0;

private:
    const JsonSchema _parameterSchema;

    T _parseParameters(const JsonValue& input) const
    {
        T inputParams;
        if (input.isEmpty())
            Json::deserialize<T>(Json::parse("{}"), inputParams);
        else
        {
            const auto errors =
                JsonSchemaValidator::validate(input, _parameterSchema);
            if (!errors.empty())
                throw std::invalid_argument(
                    "Could not parse " + getName() +
                    " parameters: " + string_utils::join(errors, ", "));

            Json::deserialize<T>(input, inputParams);
        }

        return inputParams;
    }
};

/**
 * @brief The EmptyLoaderParameters struct is a convenience type for loaders
 * that do not have any input parameter
 */
struct EmptyLoaderParameters
{
};
template <>
struct JsonAdapter<EmptyLoaderParameters>
{
    static JsonSchema getSchema(const EmptyLoaderParameters& value)
    {
        return JsonSchema();
    }

    static bool serialize(const EmptyLoaderParameters& value, JsonValue& json)
    {
        return true;
    }

    static bool deserialize(const JsonValue& json, EmptyLoaderParameters& value)
    {
        return true;
    }
};

/**
 * @brief The NoInputLoader class is a convenience class that loaders that do
 * not require any input can extend
 */
class NoInputLoader : public Loader<EmptyLoaderParameters>
{
public:
    /**
     * @brief importFromBlob imports a model from a blob of memory
     * @param blob the memory blob containing the model data
     * @param callback a callback to update the load progress to the caller
     * @return a std::vector with the loaded ModelDescriptorPtr objects
     */
    virtual std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback, Scene& scene) const = 0;

    /**
     * @brief importFromFile imports a model from a file from disk
     * @param path the path to the file on disk
     * @param callback a callback to update the load progress to the caller
     * @return a std::vector with the loaded ModelDescriptorPtr objects
     */
    virtual std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& path, const LoaderProgress& callback,
        Scene& scene) const = 0;

    std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const EmptyLoaderParameters&, Scene& scene) const final
    {
        return importFromBlob(std::move(blob), callback, scene);
    }

    std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& path, const LoaderProgress& callback,
        const EmptyLoaderParameters&, Scene& scene) const final
    {
        return importFromFile(path, callback, scene);
    }
};
} // namespace brayns
