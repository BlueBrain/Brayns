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
#include <brayns/common/utils/utils.h>

#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace brayns
{
const float TOTAL_PROGRESS = 100.f;

LoadModelFunctor::LoadModelFunctor(Engine& engine, const ModelParams& params)
    : _engine(engine)
    , _params(params)
{
}

ModelDescriptorPtr LoadModelFunctor::operator()(Blob&& blob)
{
    return _performLoad([&] { return _loadData(std::move(blob), _params); });
}

ModelDescriptorPtr LoadModelFunctor::operator()()
{
    const auto& path = _params.getPath();
    return _performLoad([&] { return _loadData(path, _params); });
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

ModelDescriptorPtr LoadModelFunctor::_loadData(Blob&& blob,
                                               const ModelParams& params)
{
    return _engine.getScene().loadModel(std::move(blob), NO_MATERIAL, params,
                                        {_getProgressFunc()});
}

ModelDescriptorPtr LoadModelFunctor::_loadData(const std::string& path,
                                               const ModelParams& params)
{
    return _engine.getScene().loadModel(path, NO_MATERIAL, params,
                                        {_getProgressFunc()});
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
        if (newProgress == 0 || newProgress % size_t(TOTAL_PROGRESS) > _nextTic)
        {
            _updateProgress(msg, newProgress - _nextTic);
            _nextTic = newProgress;
        }
    };
}
}
