/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "Errors.h"

namespace brayns
{
const float TOTAL_PROGRESS = 100.f;

LoadModelFunctor::LoadModelFunctor(Engine& engine, LoaderRegistry& registry,
                                   const ModelParams& params)
    : _engine(engine)
    , _registry(registry)
    , _params(params)
{
}

std::vector<ModelDescriptorPtr> LoadModelFunctor::operator()(Blob&& blob)
{
    return _performLoad([&] { return _loadData(std::move(blob), _params); });
}

std::vector<ModelDescriptorPtr> LoadModelFunctor::operator()()
{
    const auto& path = _params.getPath();
    return _performLoad([&] { return _loadData(path, _params); });
}

std::vector<ModelDescriptorPtr> LoadModelFunctor::_performLoad(
    const std::function<std::vector<ModelDescriptorPtr>()>& loadData)
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

std::vector<ModelDescriptorPtr> LoadModelFunctor::_loadData(
    Blob&& blob, const ModelParams& params)
{
    const auto& loader =
        _registry.getSuitableLoader("", blob.type, params.getLoaderName());
    auto models =
        loader.loadFromBlob(std::move(blob), {_getProgressFunc()},
                            params.getLoadParameters(), _engine.getScene());

    _engine.getScene().addModels(models, params);

    return models;
}

std::vector<ModelDescriptorPtr> LoadModelFunctor::_loadData(
    const std::string& path, const ModelParams& params)
{
    const auto& loader =
        _registry.getSuitableLoader(path, "", params.getLoaderName());

    auto models =
        loader.loadFromFile(path, {_getProgressFunc()},
                            params.getLoadParameters(), _engine.getScene());

    _engine.getScene().addModels(models, params);

    return models;
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
} // namespace brayns
