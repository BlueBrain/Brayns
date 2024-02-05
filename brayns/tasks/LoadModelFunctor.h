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

#pragma once

#include <brayns/common/tasks/TaskFunctor.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Model.h>
#include <brayns/io/LoaderRegistry.h>

namespace brayns
{
/**
 * A task functor which loads data from blob or file path and adds the loaded
 * data to the scene.
 */
class LoadModelFunctor : public TaskFunctor
{
public:
    LoadModelFunctor(Engine& engine, LoaderRegistry& registry,
                     const ModelParams& params);
    LoadModelFunctor(LoadModelFunctor&&) = default;
    std::vector<ModelDescriptorPtr> operator()(Blob&& blob);
    std::vector<ModelDescriptorPtr> operator()();

private:
    std::vector<ModelDescriptorPtr> _performLoad(
        const std::function<std::vector<ModelDescriptorPtr>()>& loadData);

    std::vector<ModelDescriptorPtr> _loadData(Blob&& blob,
                                              const ModelParams& params);
    std::vector<ModelDescriptorPtr> _loadData(const std::string& path,
                                              const ModelParams& params);

    void _updateProgress(const std::string& message, const size_t increment);

    std::function<void(std::string, float)> _getProgressFunc();

    Engine& _engine;
    LoaderRegistry& _registry;
    ModelParams _params;
    size_t _currentProgress{0};
    size_t _nextTic{0};
};
} // namespace brayns
