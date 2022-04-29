#include "ProgressUpdater.h"

ProgressUpdater::ProgressUpdater(const brayns::LoaderProgress &callback, const size_t numStages)
    : _callback(callback)
    , _numStages(numStages)
    , _stageSize(1.f / static_cast<float>(_numStages))
    , _currentStage(0)
    , _currentStageChunkSize(1.f)
    , _currentStageProgress(0.f)
{
}

void ProgressUpdater::beginStage(const size_t numSubElements)
{
    _currentStageChunkSize = 1.f / static_cast<float>(numSubElements);
}

void ProgressUpdater::update(const std::string &message) noexcept
{
    const auto globalProgress = (static_cast<float>(_currentStage) + _currentStageProgress) * _stageSize;
    _currentStageProgress += _currentStageChunkSize;
    _currentStageProgress = _currentStageProgress > 1.f ? 1.f : _currentStageProgress;
    _callback.updateProgress(message, globalProgress);
}

void ProgressUpdater::endStage()
{
    ++_currentStage;
    _currentStageProgress = 0.f;
}
