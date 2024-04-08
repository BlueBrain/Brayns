/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "ILoader.h"
#include "LoaderInfo.h"

namespace brayns
{
class LoaderRef
{
public:
    explicit LoaderRef(LoaderInfo info, std::unique_ptr<ILoader> loader);

    const LoaderInfo &getInfo() const;
    const std::string &getPlugin() const;
    const std::string &getName() const;
    const std::vector<std::string> &getExtensions() const;
    bool canLoadBinary() const;
    const JsonSchema &getSchema() const;
    std::vector<std::shared_ptr<Model>> loadBinary(const RawBinaryLoaderRequest &request);
    std::vector<std::shared_ptr<Model>> loadFile(const RawFileLoaderRequest &request);

private:
    LoaderInfo _info;
    std::unique_ptr<ILoader> _loader;
};

class LoaderBuilder
{
public:
    static LoaderRef build(std::string plugin, std::unique_ptr<ILoader> loader);
};

class LoaderRegistry
{
public:
    std::vector<LoaderInfo> getInfos() const;
    LoaderRef *findByName(const std::string &name);
    LoaderRef *findByFormat(const std::string &format);
    void add(std::string plugin, std::unique_ptr<ILoader> loader);

private:
    std::vector<LoaderRef> _loaders;
};

class LoaderRegistryBuilder
{
public:
    explicit LoaderRegistryBuilder(std::string plugin, LoaderRegistry &loaders);

    void addLoader(std::unique_ptr<ILoader> loader);

    template<typename T, typename... Args>
    void add(Args &&...args)
    {
        auto loader = std::make_unique<T>(std::forward<Args>(args)...);
        addLoader(std::move(loader));
    }

private:
    std::string _plugin;
    LoaderRegistry &_loaders;
};

class CoreLoaderRegistry
{
public:
    static LoaderRegistry create();
};
} // namespace brayns
