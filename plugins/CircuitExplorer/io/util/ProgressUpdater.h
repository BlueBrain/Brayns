#pragma once

#include <brayns/io/Loader.h>

class ProgressUpdater
{
public:
    ProgressUpdater(const brayns::LoaderProgress &callback, const size_t stages);

    void beginStage(const size_t numSubElements = 1);

    void update(const std::string &message) noexcept;

    void endStage();

private:
    const brayns::LoaderProgress &_callback;
    const size_t _numStages{};
    const float _stageSize{};

    size_t _currentStage{};
    float _currentStageChunkSize{};
    float _currentStageProgress{};
};
