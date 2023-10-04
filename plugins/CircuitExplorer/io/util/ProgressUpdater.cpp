#include "ProgressUpdater.h"

#include <cassert>

ProgressUpdater::ProgressUpdater(const brayns::LoaderProgress &callback, std::size_t numStages, float maxProgress):
    _callback(callback),
    _numStages(numStages),
    _stageSize(maxProgress / static_cast<float>(_numStages)),
    _currentStage(0),
    _currentStageChunkSize(1.f),
    _currentStageProgress(0.f)
{
}

void ProgressUpdater::beginStage(std::string_view message, std::size_t numSubElements)
{
    assert(_currentStageProgress == 0.f);

    _currentMessage = std::string(message);
    _currentStageChunkSize = 1.f / static_cast<float>(numSubElements);
}

void ProgressUpdater::update(std::size_t numSubElementsCompleted) noexcept
{
    const auto globalProgress = (static_cast<float>(_currentStage) + _currentStageProgress) * _stageSize;
    _currentStageProgress += _currentStageChunkSize * static_cast<float>(numSubElementsCompleted);
    _currentStageProgress = std::clamp(_currentStageProgress, 0.f, 1.f);
    _callback.updateProgress(_currentMessage, globalProgress);
}

void ProgressUpdater::endStage()
{
    ++_currentStage;
    _currentStageProgress = 0.f;
    // Send a final update after each stage to show complete progress
    update(0);
}

void ProgressUpdater::end(std::string_view message)
{
    assert(_currentStage == _numStages);

    _callback.updateProgress(std::string(message), static_cast<float>(_numStages) * _stageSize);
};
