/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "LoadModelFunctor.h"

#include "errors.h"

#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace brayns
{
const float TOTAL_PROGRESS = 100.f;

LoadModelFunctor::LoadModelFunctor(EnginePtr engine)
    : _engine(engine)
{
}

ModelDescriptorPtr LoadModelFunctor::operator()(Blob&& blob)
{
    // extract the archive and treat it as 'load from folder'
    if (isArchive(blob))
    {
        struct Scope
        {
            Scope() { fs::create_directories(_path); }
            ~Scope() { fs::remove_all(_path); }
            ModelDescriptorPtr operator()(Blob&& blob, LoadModelFunctor& parent)
            {
                extractBlob(std::move(blob), _path.string());
                return parent._performLoad(
                    [&] { return parent._loadData(_path.string()); });
            }

        private:
            fs::path _path = fs::temp_directory_path() / fs::unique_path();
        } scope;
        return scope(std::move(blob), *this);
    }

    return _performLoad([&] { return _loadData(std::move(blob)); });
}

ModelDescriptorPtr LoadModelFunctor::operator()(const std::string& path)
{
    // extract the archive and treat it as 'load from folder'
    if (isArchive(path))
    {
        struct Scope
        {
            Scope() { fs::create_directories(_path); }
            ~Scope() { fs::remove_all(_path); }
            ModelDescriptorPtr operator()(const std::string& file,
                                          LoadModelFunctor& parent)
            {
                extractFile(file, _path.string());
                return parent._performLoad(
                    [&] { return parent._loadData(_path.string()); });
            }

        private:
            fs::path _path = fs::temp_directory_path() / fs::unique_path();
        } scope;
        return scope(path, *this);
    }

    return _performLoad([&] { return _loadData(path); });
}

ModelDescriptorPtr LoadModelFunctor::_performLoad(
    const std::function<ModelDescriptorPtr()>& loadData)
{
    try
    {
        return loadData();
    }
    catch (const std::exception& e)
    {
        progress("Loading failed",
                 (TOTAL_PROGRESS - _currentProgress) / TOTAL_PROGRESS, 1.f);
        throw LOADING_BINARY_FAILED(e.what());
    }
}

ModelDescriptorPtr LoadModelFunctor::_loadData(Blob&& blob)
{
    return _engine->getScene().load(std::move(blob), Transformation{},
                                    NO_MATERIAL, _getProgressFunc());
}

ModelDescriptorPtr LoadModelFunctor::_loadData(const std::string& path)
{
    return _engine->getScene().load(path, Transformation{}, NO_MATERIAL,
                                    _getProgressFunc());
}

void LoadModelFunctor::_updateProgress(const std::string& message,
                                       const size_t increment)
{
    _currentProgress += increment;
    progress(message, increment / TOTAL_PROGRESS,
             _currentProgress / TOTAL_PROGRESS);
}

std::function<void(std::string, float)> LoadModelFunctor::_getProgressFunc()
{
    return [this](const std::string& msg, const float progress) {
        cancelCheck();
        const size_t newProgress = progress * TOTAL_PROGRESS;
        if (newProgress % size_t(TOTAL_PROGRESS) > _nextTic)
        {
            _updateProgress(msg, newProgress - _nextTic);
            _nextTic = newProgress;
        }
    };
}
}
