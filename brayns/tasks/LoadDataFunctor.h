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

#pragma once

#include <brayns/common/tasks/TaskFunctor.h>
#include <brayns/common/types.h>

#include <shared_mutex>

namespace brayns
{
/**
 * A task functor which loads data from blob or file path and adds the loaded
 * data to the scene.
 */
class LoadDataFunctor : public TaskFunctor
{
public:
    LoadDataFunctor(EnginePtr engine);
    ~LoadDataFunctor();
    LoadDataFunctor(LoadDataFunctor&&) = default;
    void operator()(Blob&& blob);
    void operator()(const std::string& path);

private:
    void _performLoad(const std::function<void()>& loadData);

    void _loadData(Blob&& blob);
    void _loadData(const std::string& path);

    void _postLoad(bool cancellable = true);

    void _updateProgress(const std::string& message, const size_t increment);

    std::function<void(std::string, float)> _getProgressFunc();

    EnginePtr _engine;
    bool _loadDefaultScene{false};
    size_t _currentProgress{0};
    size_t _nextTic{0};
    std::unique_lock<std::shared_timed_mutex> _lock;
};
}
