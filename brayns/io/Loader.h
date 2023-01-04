/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/model/Model.h>
#include <brayns/json/Json.h>
#include <brayns/json/JsonSchema.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/string/StringJoiner.h>

#include <functional>
#include <string>
#include <vector>

namespace brayns
{
/**
 * @brief Model binary data.
 *
 */
struct Blob
{
    /**
     * @brief File type.
     *
     */
    std::string type;

    /**
     * @brief Loader name.
     *
     */
    std::string name;

    /**
     * @brief Binary data.
     *
     */
    std::vector<uint8_t> data;
};

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
    using CallbackFn = std::function<void(const std::string &, float)>;

    explicit LoaderProgress(CallbackFn callback);

    LoaderProgress() = default;
    ~LoaderProgress() = default;

    /**
     * Update the current progress of an operation and call the callback
     */
    void updateProgress(const std::string &message, const float fraction) const;

private:
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
     * @brief Query the loader to check if a file can be loaded. Filename and file extension (without the dot)
     * are checked.
     * @param path Path to the file.
     * @param extension Extracted file extension from path (without the dot)
     */
    virtual bool isSupported(const std::string &path, const std::string &extension) const;

    /**
     * @brief Returns the loader identificative name.
     * @return std::string.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief returns the loader input parameter schema.
     * @returns a JsonSchema object.
     */
    virtual const JsonSchema &getInputParametersSchema() const = 0;

    /**
     * @brief Loads a list of models from a blob of bytes.
     * @param blob The byte data.
     * @param callback A callback to update the progress to the caller.
     * @param params A JSON object with the parameters to configure the load.
     * @return a list of loaded models.
     */
    virtual std::vector<std::shared_ptr<Model>>
        loadFromBlob(const Blob &blob, const LoaderProgress &callback, const JsonValue &params) const = 0;

    /**
     * @brief Loads a list of models from a file.
     * @param path Path to the file on the current filesystem.
     * @param callback A callback to update the progress to the caller.
     * @param params A JSON object with the parameters to configure the load.
     * @return a list of loaded models.
     */
    virtual std::vector<std::shared_ptr<Model>>
        loadFromFile(const std::string &path, const LoaderProgress &callback, const JsonValue &params) const = 0;
};

/**
 * A base class for data loaders to unify loading data from blobs and files, and
 * provide progress feedback.
 */
template<typename T>
class Loader : public AbstractLoader
{
public:
    Loader()
        : _parameterSchema(Json::getSchema<T>())
    {
    }

    virtual ~Loader() = default;

    virtual const JsonSchema &getInputParametersSchema() const override
    {
        return _parameterSchema;
    }

    virtual std::vector<std::shared_ptr<Model>>
        loadFromBlob(const Blob &blob, const LoaderProgress &callback, const JsonValue &params) const override
    {
        const T inputParams = _parseParameters(params);
        return importFromBlob(blob, callback, inputParams);
    }

    virtual std::vector<std::shared_ptr<Model>>
        loadFromFile(const std::string &path, const LoaderProgress &callback, const JsonValue &params) const override
    {
        const T inputParams = _parseParameters(params);
        return importFromFile(path, callback, inputParams);
    }

    /**
     * @copydoc AbstractLoader::loadFromBlob()
     *
     * @param blob The byte data.
     * @param callback A callback to update the progress to the caller.
     * @param params Parameters to configure the load.
     * @return a list of loaded models.
     */
    virtual std::vector<std::shared_ptr<Model>>
        importFromBlob(const Blob &blob, const LoaderProgress &callback, const T &params) const = 0;

    /**
     * @copydoc AbstractLoader::loadFromFile()
     *
     * @param path Path to the file on the current filesystem.
     * @param callback A callback to update the progress to the caller.
     * @param params Parameters to configure the load.
     * @return a list of loaded models.
     */
    virtual std::vector<std::shared_ptr<Model>>
        importFromFile(const std::string &path, const LoaderProgress &callback, const T &params) const = 0;

private:
    const JsonSchema _parameterSchema;

    T _parseParameters(const JsonValue &input) const
    {
        if (input.isEmpty())
        {
            return Json::deserialize<T>(Json::parse("{}"));
        }

        auto errors = JsonSchemaValidator::validate(input, _parameterSchema);
        if (!errors.empty())
        {
            auto message = "Cannot parse " + getName() + " parameters: " + StringJoiner::join(errors, ", ");
            throw std::invalid_argument(message);
        }

        return Json::deserialize<T>(input);
    }
};

/**
 * @brief Convenience type for loaders that do not have load parameters.
 */
struct EmptyLoaderParameters
{
};

template<>
struct JsonAdapter<EmptyLoaderParameters>
{
    static JsonSchema getSchema();
    static void serialize(const EmptyLoaderParameters &params, JsonValue &value);
    static void deserialize(const JsonValue &value, EmptyLoaderParameters &params);
};

/**
 * @brief Base class for loaders that do not have load parameters.
 */
class NoInputLoader : public Loader<EmptyLoaderParameters>
{
public:
    /**
     * @copydoc AbstractLoader::loadFromBlob()
     * @param blob The byte data.
     * @param ccallback A callback to update the progress to the caller.
     * @return a list of loaded models.
     */
    virtual std::vector<std::shared_ptr<Model>> importFromBlob(const Blob &blob, const LoaderProgress &callback)
        const = 0;

    /**
     * @copydoc AbstractLoader::loadFromFile()
     * @param path Path to the file on the current filesystem.
     * @param callback A callback to update the progress to the caller.
     * @return a list of loaded models.
     */
    virtual std::vector<std::shared_ptr<Model>> importFromFile(const std::string &path, const LoaderProgress &callback)
        const = 0;

    std::vector<std::shared_ptr<Model>> importFromBlob(
        const Blob &blob,
        const LoaderProgress &callback,
        const EmptyLoaderParameters &parameters) const final;

    std::vector<std::shared_ptr<Model>> importFromFile(
        const std::string &path,
        const LoaderProgress &callback,
        const EmptyLoaderParameters &parameters) const final;
};
} // namespace brayns
