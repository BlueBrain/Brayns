/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#pragma once

#include <memory>
#include <unordered_map>

#include <brayns/io/Loader.h>
#include <brayns/io/loaders/mesh/MeshParser.h>

namespace brayns
{
/**
 * @brief Helper class to store mesh parsers per format.
 *
 */
class MeshParserRegistry
{
public:
    /**
     * @brief Use the registered parsers to get all supported formats.
     *
     * @return std::vector<std::string> List of file extensions.
     */
    std::vector<std::string> getAllSupportedExtensions() const;

    /**
     * @brief Get the parser supporting the given format.
     *
     * @param extension File extension.
     * @return const MeshParser& Parser that can handle the file.
     * @throw std::runtime_error Format not supported.
     */
    const MeshParser &getParser(const std::string &extension) const;

    /**
     * @brief Find if a parser can support the extension and return it.
     *
     * @param extension File extension.
     * @return const MeshParser* Parser or null if not supported.
     */
    const MeshParser *findParser(const std::string &extension) const;

    /**
     * @brief Register a new mesh parser.
     *
     * Throw if one of the parser formats is already registered.
     *
     * @param parser Parser to register.
     * @throw std::runtime_error Another parser can support one of the formats.
     */
    void addParser(std::shared_ptr<MeshParser> parser);

    /**
     * @brief Shortcut to add a parser of the given type.
     *
     * @tparam T Parser type.
     * @tparam Args Parser constructor arguments types.
     * @param args Parser constructor arguments.
     */
    template<typename T, typename... Args>
    void add(Args &&...args)
    {
        auto parser = std::make_shared<T>(std::forward<Args>(args)...);
        addParser(std::move(parser));
    }

private:
    std::unordered_map<std::string, std::shared_ptr<MeshParser>> _parsers;
};

/**
 * @brief Loader to support mesh files.
 *
 * Multiple objects are merged in a single mesh.
 *
 * Vertices are duplicated (vertex count = 3 * index count).
 *
 * Only triangular faces are supported.
 *
 * Materials are not extracted.
 *
 * A default white material is used for the mesh model.
 *
 */
class MeshLoader : public Loader<EmptyLoaderParams>
{
public:
    /**
     * @brief Register all parsers.
     *
     */
    MeshLoader();

    /**
     * @brief Get the loader name.
     *
     * @return std::string Loader name.
     */
    std::string getName() const override;

    /**
     * @brief Extract all supported formats using the registered parsers.
     *
     * @return std::vector<std::string> Supported file extensions.
     */
    std::vector<std::string> getExtensions() const override;

    /**
     * @brief Supports binary
     *
     * @return true
     */
    bool canLoadBinary() const override;

    /**
     * @brief Import the mesh in the given scene from the given binary data.
     *
     * @param request Loading request.
     * @return std::vector<ModelDescriptorPtr> Models containing the mesh.
     * @throw std::runtime_error An error occurs.
     */
    std::vector<std::shared_ptr<Model>> loadBinary(const BinaryRequest &request) override;

private:
    MeshParserRegistry _parsers;
};
} // namespace brayns
