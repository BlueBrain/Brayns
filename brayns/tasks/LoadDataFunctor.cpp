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

#include "LoadDataFunctor.h"

#include "errors.h"

#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace brayns
{
const float TOTAL_PROGRESS = 100.f;

LoadDataFunctor::LoadDataFunctor(EnginePtr engine)
    : _engine(engine)
{
}

void LoadDataFunctor::operator()(Blob&& blob)
{
    // extract the archive and treat it as 'load from folder'
    if (isArchive(blob))
    {
        struct Scope
        {
            Scope(Blob&& blob, LoadDataFunctor& parent)
            {
                fs::create_directories(path);

                extractBlob(std::move(blob), path.string());
                parent._performLoad([&] { parent._loadData(path.string()); });
            }

            ~Scope() { fs::remove_all(path); }
            fs::path path = fs::temp_directory_path() / fs::unique_path();
        } scope(std::move(blob), *this);
        return;
    }

    _performLoad([&] { _loadData(std::move(blob)); });
}

void LoadDataFunctor::operator()(const std::string& path)
{
    // extract the archive and treat it as 'load from folder'
    if (isArchive(path))
    {
        struct Scope
        {
            Scope(const std::string& file, LoadDataFunctor& parent)
            {
                fs::create_directories(path);

                extractFile(file, path.string());
                parent._performLoad([&] { parent._loadData(path.string()); });
            }

            ~Scope() { fs::remove_all(path); }
            fs::path path = fs::temp_directory_path() / fs::unique_path();
        } scope(path, *this);
        return;
    }

    _performLoad([&] { _loadData(path); });
}

void LoadDataFunctor::_performLoad(const std::function<void()>& loadData)
{
    try
    {
        loadData();
    }
    catch (const std::exception& e)
    {
        progress("Loading failed",
                 (TOTAL_PROGRESS - _currentProgress) / TOTAL_PROGRESS, 1.f);
        throw LOADING_BINARY_FAILED(e.what());
    }
}

void LoadDataFunctor::_loadData(Blob&& blob)
{
    _engine->getScene().load(std::move(blob), Matrix4f(), NO_MATERIAL,
                             _getProgressFunc());
}

void LoadDataFunctor::_loadData(const std::string& path)
{
    _engine->getScene().load(path, Matrix4f(), NO_MATERIAL, _getProgressFunc());
}

void LoadDataFunctor::_updateProgress(const std::string& message,
                                      const size_t increment)
{
    _currentProgress += increment;
    progress(message, increment / TOTAL_PROGRESS,
             _currentProgress / TOTAL_PROGRESS);
}

std::function<void(std::string, float)> LoadDataFunctor::_getProgressFunc()
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
