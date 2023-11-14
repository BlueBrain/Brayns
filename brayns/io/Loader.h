/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <stdexcept>

#include "EmptyLoaderParams.h"
#include "ILoader.h"
#include "LoaderFormat.h"

#include <brayns/utils/FileReader.h>

namespace brayns
{
template<typename T>
struct BinaryLoaderRequest
{
    std::string_view format;
    std::string_view data;
    LoaderProgress progress = [](const auto &, auto) {};
    T params{};
};

template<typename T>
struct FileLoaderRequest
{
    std::string_view path;
    LoaderProgress progress = [](const auto &, auto) {};
    T params{};
};

template<typename ParamsType = EmptyLoaderParams>
class Loader : public ILoader
{
public:
    using Params = ParamsType;
    using BinaryRequest = BinaryLoaderRequest<Params>;
    using FileRequest = FileLoaderRequest<Params>;

    virtual std::vector<std::shared_ptr<Model>> loadBinary(const BinaryRequest &request)
    {
        (void)request;
        throw std::runtime_error("Binary not supported");
    }

    virtual std::vector<std::shared_ptr<Model>> loadFile(const FileRequest &request)
    {
        auto path = std::string(request.path);
        auto format = LoaderFormat::fromPath(path);
        auto data = FileReader::read(path);
        auto binary = BinaryRequest();
        binary.format = format;
        binary.data = data;
        binary.progress = request.progress;
        binary.params = request.params;
        return loadBinary(binary);
    }

    JsonSchema getSchema() const override
    {
        return Json::getSchema<Params>();
    }

    bool canLoadBinary() const override
    {
        return false;
    }

    std::vector<std::shared_ptr<Model>> loadBinary(const RawBinaryLoaderRequest &request) override
    {
        auto parsed = BinaryRequest();
        parsed.format = request.format;
        parsed.data = request.data;
        parsed.progress = request.progress;
        parsed.params = Json::deserialize<Params>(request.params);
        return loadBinary(parsed);
    }

    std::vector<std::shared_ptr<Model>> loadFile(const RawFileLoaderRequest &request) override
    {
        auto parsed = FileRequest();
        parsed.path = request.path;
        parsed.progress = request.progress;
        parsed.params = Json::deserialize<Params>(request.params);
        return loadFile(parsed);
    }
};
} // namespace brayns
