/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/common/propertymap/PropertyMap.h>
#include <brayns/common/types.h>
#include <brayns/json/Json.h>
#include <brayns/json/JsonSchema.h>
#include <brayns/utils/StringUtils.h>

#include <functional>

#ifdef BRAYNS_USE_OPENMP
#include <omp.h>
#endif

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
#ifdef BRAYNS_USE_OPENMP
        if (omp_get_thread_num() == 0)
#endif
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
     * @brief Query the loader if it can load the given file. Compares the file extension
     * to the supported extensions (removing the dot, if any, and making them lowercase).
     * Can be overriden to perform mor explicit checks
     */
    virtual bool isSupported(const std::string& filename,
                             const std::string& extension) const
    {
        const auto extensions = getSupportedExtensions();
        const auto lcExtension = string_utils::toLowercase(extension[0] == '.'? extension.substr(1) : extension);
        auto it = std::find_if(extensions.begin(), extensions.end(), [&](const std::string& ext)
        {
            return string_utils::toLowercase(ext[0] == '.'? ext.substr(1) : ext) == lcExtension;
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
    virtual JsonSchema getInputParametersSchema() = 0;

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
        Blob&& blob, const LoaderProgress& callback,
        const JsonValue& params) const = 0;

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
        const JsonValue& params) const = 0;
};

/**
 * A base class for data loaders to unify loading data from blobs and files, and
 * provide progress feedback.
 */
template <typename T>
class Loader : public AbstractLoader
{
public:
    Loader(Scene& scene)
        : _scene(scene)
    {
    }

    virtual ~Loader() = default;

    virtual JsonSchema getInputParametersSchema() override
    {
        return Json::getSchema<T>();
    }

    virtual std::vector<ModelDescriptorPtr> loadFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const JsonValue& params) const override
    {
        T inputParams;
        if (!Json::deserialize<T>(params, inputParams))
            throw std::invalid_argument("Could not parse " + getName() +
                                        " loader parameters");

        return importFromBlob(std::move(blob), callback, inputParams);
    }

    virtual std::vector<ModelDescriptorPtr> loadFromFile(
        const std::string& path, const LoaderProgress& callback,
        const JsonValue& params) const override
    {
        T inputParams;
        if (!Json::deserialize<T>(params, inputParams))
            throw std::invalid_argument("Could not parse " + getName() +
                                        " loader parameters");

        return importFromFile(path, callback, inputParams);
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
        Blob&& blob, const LoaderProgress& callback,
        const T& properties) const = 0;

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
        const T& properties) const = 0;

protected:
    Scene& _scene;
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
    NoInputLoader(Scene& scene)
        : Loader(scene)
    {
    }

    /**
     * @brief importFromBlob imports a model from a blob of memory
     * @param blob the memory blob containing the model data
     * @param callback a callback to update the load progress to the caller
     * @return a std::vector with the loaded ModelDescriptorPtr objects
     */
    virtual std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback) const = 0;

    /**
     * @brief importFromFile imports a model from a file from disk
     * @param path the path to the file on disk
     * @param callback a callback to update the load progress to the caller
     * @return a std::vector with the loaded ModelDescriptorPtr objects
     */
    virtual std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& path, const LoaderProgress& callback) const = 0;

    std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const EmptyLoaderParameters&) const final
    {
        return importFromBlob(std::move(blob), callback);
    }

    std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& path, const LoaderProgress& callback,
        const EmptyLoaderParameters&) const final
    {
        return importFromFile(path, callback);
    }
};
} // namespace brayns
