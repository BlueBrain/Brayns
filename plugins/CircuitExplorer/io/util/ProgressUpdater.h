#pragma once

#include <brayns/io/Loader.h>

class ProgressUpdater
{
public:
    ProgressUpdater(const brayns::LoaderProgress &callback, std::size_t stages, float maxProgress = 1.f);

    void beginStage(std::string_view message, std::size_t numSubElements = 1);
    void update(std::size_t numSubElementsCompleted = 1) noexcept;
    void endStage();
    void end(std::string_view message);

private:
    const brayns::LoaderProgress &_callback;
    std::size_t _numStages;
    float _stageSize;

    std::size_t _currentStage{};
    float _currentStageChunkSize{};
    float _currentStageProgress{};
    std::string _currentMessage;
};
