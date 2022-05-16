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

#include "Loader.h"

namespace brayns
{
LoaderProgress::LoaderProgress(CallbackFn callback)
    : _callback(std::move(callback))
{
}

void LoaderProgress::updateProgress(const std::string &message, const float fraction) const
{
    if (_callback)
        _callback(message, fraction);
}

bool AbstractLoader::isSupported(const std::string &fileName, const std::string &extension) const
{
    (void)fileName;

    const auto extensions = getSupportedExtensions();
    const auto lcExtension = string_utils::toLowercase(extension[0] == '.' ? extension.substr(1) : extension);
    auto it = std::find_if(
        extensions.begin(),
        extensions.end(),
        [&](const std::string &ext)
        { return string_utils::toLowercase(ext[0] == '.' ? ext.substr(1) : ext) == lcExtension; });
    return it != extensions.end();
}

JsonSchema JsonAdapter<EmptyLoaderParameters>::getSchema()
{
    return JsonSchema();
}

bool JsonAdapter<EmptyLoaderParameters>::serialize(const EmptyLoaderParameters &params, JsonValue &value)
{
    (void)params;
    (void)value;
    return true;
}

bool JsonAdapter<EmptyLoaderParameters>::deserialize(const JsonValue &value, EmptyLoaderParameters &params)
{
    (void)value;
    (void)params;
    return true;
}

std::vector<std::unique_ptr<Model>> NoInputLoader::importFromBlob(
    Blob &&blob,
    const LoaderProgress &callback,
    const EmptyLoaderParameters &parameters) const
{
    (void)parameters;
    return importFromBlob(std::move(blob), callback);
}

std::vector<std::unique_ptr<Model>> NoInputLoader::importFromFile(
    const std::string &path,
    const LoaderProgress &callback,
    const EmptyLoaderParameters &parameters) const
{
    (void)parameters;
    return importFromFile(path, callback);
}
} // namespace brayns
