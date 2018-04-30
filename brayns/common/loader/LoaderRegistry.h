/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

namespace brayns
{
/**
 * Holds information about registered loaders and helps invoking the appropriate
 * loader for a given blob or file.
 */
class LoaderRegistry
{
public:
    struct LoaderInfo
    {
        /**
         * The function that returns the supported types (extension, filename
         * patterns) of the loader.
         */
        std::function<std::set<std::string>()> supportedTypes;

        /** The function to create the loader. */
        std::function<LoaderPtr()> createLoader;
    };

    /** Register the given loader. */
    void registerLoader(LoaderInfo loaderInfo);

    /**
     * @return true if any of the registered loaders can handle the given type
     *         (extension, filename).
     */
    bool isSupported(const std::string& type) const;

    /** @return supported types from all registered loaders. */
    std::set<std::string> supportedTypes() const;

    /**
     * Load the given blob into the given scene by choosing the first matching
     * loader based on the blob's type.
     *
     * @param blob the blob containing the data to import
     * @param scene the scene where to add the loaded model to
     * @param transformation the transformation to apply for the added model
     * @param materialID the default material ot use
     * @param cb the callback for progress updates from the loader
     */
    void load(Blob&& blob, Scene& scene, const Matrix4f& transformation,
              const size_t materialID, Loader::UpdateCallback cb);

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
              Loader::UpdateCallback cb);

private:
    std::vector<LoaderInfo> _loaders;

    bool _isSupported(const LoaderInfo& loader, const std::string& type) const;
};
}
