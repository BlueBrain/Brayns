/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace brayns
{
using LoaderProgress = std::function<void(const std::string &, float)>;

struct RawBinaryLoaderRequest
{
    std::string_view format;
    std::string_view data;
    LoaderProgress progress = [](const auto &, auto) {};
    JsonValue params;
};

struct RawFileLoaderRequest
{
    std::string_view path;
    LoaderProgress progress = [](const auto &, auto) {};
    JsonValue params;
};

class ILoader
{
public:
    virtual ~ILoader() = default;

    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getExtensions() const = 0;
    virtual JsonSchema getSchema() const = 0;
    virtual bool canLoadBinary() const = 0;
    virtual std::vector<std::shared_ptr<Model>> loadBinary(const RawBinaryLoaderRequest &request) = 0;
    virtual std::vector<std::shared_ptr<Model>> loadFile(const RawFileLoaderRequest &request) = 0;
};

class LoaderFormat
{
public:
    static std::string from(const std::filesystem::path &path)
    {
        return path.extension().lexically_normal().string();
    }
};
} // namespace brayns
