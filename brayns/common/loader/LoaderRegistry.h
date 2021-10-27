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

#include <brayns/common/loader/Loader.h>
#include <brayns/common/propertymap/PropertyMap.h>

#include <set>

namespace brayns
{
struct LoaderInfo
{
    std::string name;
    std::vector<std::string> extensions;
    JsonSchema inputParametersSchema;
};

/**
 * Holds information about registered loaders and helps invoking the appropriate
 * loader for a given blob or file.
 */
class LoaderRegistry
{
public:
    /** Register the given loader. */
    void registerLoader(std::unique_ptr<AbstractLoader> loader);

    /**
     * Get a list of loaders and their supported file extensions and properties
     */
    const std::vector<LoaderInfo>& getLoaderInfos() const;

    /**
     * @return true if any of the registered loaders can handle the given file
     */
    bool isSupportedFile(const std::string& filename) const;

    /**
     * @return true if any of the registered loaders can handle the given type
     */
    bool isSupportedType(const std::string& type) const;

    /**
     * Get a loader that matches the provided name, filetype or loader name.
     * @throw std::runtime_error if no loader found.
     */
    const AbstractLoader& getSuitableLoader(
        const std::string& filename, const std::string& filetype,
        const std::string& loaderName) const;

    /**
     * Load the given file or folder into the given scene by choosing the first
     * matching loader based on the filename or filetype.
     *
     * @param path the file or folder containing the data to import
     * @param scene the scene where to add the loaded model to
     * @param transformation the transformation to apply for the added model
     * @param materialID the default material ot use
     * @param cb the callback for progress updates from the loader
     */
    void load(const std::string& path, Scene& scene,
              const Matrix4f& transformation, const size_t materialID,
              LoaderProgress cb);

    /** @internal */
    void clear();

private:
    std::vector<std::unique_ptr<AbstractLoader>> _loaders;
    std::vector<LoaderInfo> _loaderInfos;
};
} // namespace brayns
