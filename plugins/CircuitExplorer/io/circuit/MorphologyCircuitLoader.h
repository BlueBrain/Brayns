#pragma once

#include <brayns/common/MathTypes.h>

#include <brayns/engine/Model.h>

#include <api/IColorData.h>
#include <io/NeuronMorphologyLoaderParameters.h>
#include <io/simulation/SimulationMapping.h>
#include <io/util/ProgressUpdater.h>

/**
 * @brief The MorphologyCircuitLoader struct loads a morphology circuit into a MorphologyCircuitComponent
 */
struct MorphologyCircuitLoader
{
    struct Context
    {
        Context(const std::vector<uint64_t> &ids,
                const std::vector<std::string> &morphologyPaths,
                const std::vector<brayns::Vector3f> &positions,
                const std::vector<brayns::Quaternion> &rotations,
                const NeuronMorphologyLoaderParameters &morphologyParams);

        const std::vector<uint64_t> &ids;
        const std::vector<std::string> &morphologyPaths;
        const std::vector<brayns::Vector3f> &positions;
        const std::vector<brayns::Quaternion> &rotations;
        const NeuronMorphologyLoaderParameters &morphologyParams;
    };

    static std::vector<CompartmentStructure> load(
            const Context &context, brayns::Model &model, ProgressUpdater &cb, std::unique_ptr<IColorData> colorData);
};
