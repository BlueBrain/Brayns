#include "SomaCircuitLoader.h"

#include <components/CircuitColorComponent.h>
#include <io/circuit/colorhandlers/SomaColorHandler.h>
#include <io/circuit/components/SomaCircuitComponent.h>

SomaCircuitLoader::Context::Context(
        const std::vector<uint64_t> &ids, const std::vector<brayns::Vector3f> &positions, float radius)
 : ids(ids)
 , positions(positions)
 , radius(radius)
{
}

std::vector<CompartmentStructure> SomaCircuitLoader::load(
        const Context &context, brayns::Model &model, std::unique_ptr<IColorData> colorData)
{
    const auto &ids = context.ids;
    const auto &positions = context.positions;
    const auto radius = context.radius;

    std::vector<CompartmentStructure> result (ids.size());

    std::vector<brayns::Sphere> geometry (ids.size());

    #pragma omp parallel for
    for(size_t i = 0; i < ids.size(); ++i)
    {
        const auto &pos = positions[i];
        auto &somaSphere = geometry[i];
        somaSphere.center = pos;
        somaSphere.radius = radius;

        auto &compartment = result[i];

        compartment.numItems = 1;
        compartment.sectionSegments[-1].push_back(0);
    }

    auto &somaCircuit = model.addComponent<SomaCircuitComponent>();
    somaCircuit.setSomas(ids, std::move(geometry));

    auto colorHandler = std::make_unique<SomaColorHandler>(somaCircuit);
    model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));

    return result;
}
