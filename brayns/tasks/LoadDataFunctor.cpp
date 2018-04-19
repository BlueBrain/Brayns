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

#include <brayns/io/MeshLoader.h>
#include <brayns/io/XYZBLoader.h>

#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>

namespace brayns
{
const size_t LOADING_PROGRESS_DATA = 100;
const size_t LOADING_PROGRESS_STEP = 10;
const float TOTAL_PROGRESS = 3 * LOADING_PROGRESS_STEP + LOADING_PROGRESS_DATA;

LoadDataFunctor::LoadDataFunctor(EnginePtr engine)
    : _engine(engine)
    , _lock{_engine->dataMutex(), std::defer_lock}
{
}

LoadDataFunctor::~LoadDataFunctor()
{
    if (!_loadDefaultScene)
        return;

    // load default scene if we got cancelled or any other error occurred
    Scene& scene = _engine->getScene();
    scene.unload();
    BRAYNS_INFO << "Building default scene" << std::endl;
    scene.buildDefault();

    _postLoad(false);
}

void LoadDataFunctor::operator()(Blob&& blob)
{
    _performLoad([&] { _loadData(std::move(blob)); });
}

void LoadDataFunctor::operator()(const std::string& path)
{
    _performLoad([&] { _loadData(path); });
}

void LoadDataFunctor::_performLoad(const std::function<void()>& loadData)
{
    try
    {
        // fix race condition: we need exclusive access to the scene as we
        // unload the current one. So no rendering & snapshot must occur.
        while (!_lock.try_lock_for(std::chrono::seconds(1)))
            _updateProgress("Waiting for scene access ...", 0.f);

        _updateProgress("Unloading ...", 0.f);
        Scene& scene = _engine->getScene();
        scene.unload();
        _loadDefaultScene = true;

        _updateProgress("Loading data ...", LOADING_PROGRESS_STEP);
        scene.resetMaterials();
        try
        {
            loadData();
        }
        catch (const std::exception& e)
        {
            throw LOADING_BINARY_FAILED(e.what());
        }

        if (!scene.empty())
            _postLoad();
        _loadDefaultScene = false;
    }
    catch (...)
    {
        progress("Loading failed",
                 (TOTAL_PROGRESS - _currentProgress) / TOTAL_PROGRESS, 1.f);
        throw;
    }
}

void LoadDataFunctor::_loadData(Blob&& blob)
{
    // for unit tests
    if (_forever(blob.type))
        return;

    if (blob.type == "xyz")
        _loadXYZBBlob(std::move(blob));
    else
        _loadMeshBlob(std::move(blob));
}

void LoadDataFunctor::_loadXYZBBlob(Blob&& blob)
{
    auto& scene = _engine->getScene();
    XYZBLoader xyzbLoader(
        _engine->getParametersManager().getGeometryParameters());
    xyzbLoader.setProgressCallback(_getProgressFunc());
    xyzbLoader.importFromBlob(blob, scene);
}

void LoadDataFunctor::_loadMeshBlob(Blob&& blob)
{
    const auto& geometryParameters =
        _engine->getParametersManager().getGeometryParameters();
    auto& scene = _engine->getScene();
    const size_t material =
        geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
            ? NB_SYSTEM_MATERIALS
            : NO_MATERIAL;
    MeshLoader meshLoader(geometryParameters);
    meshLoader.setProgressCallback(_getProgressFunc());
    meshLoader.importMeshFromBlob(blob, scene, Matrix4f(), material);
}

void LoadDataFunctor::_loadData(const std::string& path)
{
    if (_forever(path))
        return;

    auto extension = boost::filesystem::extension(path);
    if (extension.empty())
        return;

    extension = extension.erase(0, 1);
    if (extension == "xyz")
        _loadXYZBFile(path);
    else
        _loadMeshFile(path);
}

void LoadDataFunctor::_loadXYZBFile(const std::string& path)
{
    auto& scene = _engine->getScene();
    XYZBLoader xyzbLoader(
        _engine->getParametersManager().getGeometryParameters());
    xyzbLoader.setProgressCallback(_getProgressFunc());
    xyzbLoader.importFromFile(path, scene);
}

void LoadDataFunctor::_loadMeshFile(const std::string& path)
{
    const auto& geometryParameters =
        _engine->getParametersManager().getGeometryParameters();
    auto& scene = _engine->getScene();
    const size_t material =
        geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
            ? NB_SYSTEM_MATERIALS
            : NO_MATERIAL;
    MeshLoader meshLoader(geometryParameters);
    meshLoader.setProgressCallback(_getProgressFunc());
    meshLoader.importMeshFromFile(path, scene, Matrix4f(), material);
}

void LoadDataFunctor::_postLoad(const bool cancellable)
{
    Scene& scene = _engine->getScene();

    scene.buildEnvironment();

    if (cancellable)
        _updateProgress("Building geometry ...", LOADING_PROGRESS_STEP);
    scene.buildGeometry();

    const auto& geomParams =
        _engine->getParametersManager().getGeometryParameters();
    if (geomParams.getLoadCacheFile().empty() &&
        !geomParams.getSaveCacheFile().empty())
    {
        scene.saveToCacheFile();
    }

    if (cancellable)
        _updateProgress("Building acceleration structure ...",
                        LOADING_PROGRESS_STEP);
    scene.commit();

    BRAYNS_INFO << "Now rendering ..." << std::endl;

    const auto frameSize = Vector2f(_engine->getFrameBuffer().getSize());

    auto& camera = _engine->getCamera();
    camera.setInitialState(_engine->getScene().getWorldBounds());
    camera.setAspectRatio(frameSize.x() / frameSize.y());
    _engine->triggerRender();
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
        const size_t newProgress = progress * LOADING_PROGRESS_DATA;
        if (newProgress % LOADING_PROGRESS_DATA > _nextTic)
        {
            _updateProgress(msg, newProgress - _nextTic);
            _nextTic = newProgress;
        }
    };
}

bool LoadDataFunctor::_forever(const std::string& name) const
{
    if (name != "forever")
        return false;

    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        cancelCheck();
    }
    return true;
}
}
