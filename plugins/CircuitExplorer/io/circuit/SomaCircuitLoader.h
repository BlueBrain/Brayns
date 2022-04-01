#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Model.h>

#include <api/IColorData.h>
#include <io/simulation/SimulationMapping.h>

/**
 * @brief The SomaCircuitLoader struct loads a soma circuit into a SomaCircuitComponent
 */
struct SomaCircuitLoader
{
    struct Context
    {
        Context(const std::vector<uint64_t> &ids, const std::vector<brayns::Vector3f> &positions, float radius);

        const std::vector<uint64_t> &ids;
        const std::vector<brayns::Vector3f> &positions;
        const float radius;
    };

    static std::vector<CompartmentStructure> load(
            const Context &context, brayns::Model &model, std::unique_ptr<IColorData> colorData);
};
