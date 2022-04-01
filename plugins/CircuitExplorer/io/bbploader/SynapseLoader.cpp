#include "SynapseLoader.h"

#include <components/CircuitColorComponent.h>
#include <io/bbploader/colordata/BBPSynapseColorData.h>
#include <io/synapse/colorhandlers/SynapseColorHandler.h>
#include <io/synapse/components/SynapseComponent.h>

#include <brain/synapse.h>
#include <brain/synapses.h>
#include <brain/synapsesIterator.h>
#include <brain/synapsesStream.h>

namespace
{
struct AfferentLoader
{
    static void load(const bbploader::LoadContext &context, brayns::Model &model)
    {
        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        auto &synapses = model.addComponent<SynapseComponent>();

        std::map<uint32_t, std::vector<brayns::Sphere>> synapseGeometry;
        const brain::Synapses synapseData (circuit.getAfferentSynapses(gids));
        for(const auto &synapse : synapseData)
        {
            const auto position = synapse.getPostsynapticSurfacePosition();
            const auto gid = synapse.getPostsynapticGID();
            auto &buffer = synapseGeometry[gid];
            buffer.push_back({position, 2.f});
        }

        for(auto &[gid, synapseSpheres] : synapseGeometry)
        {
            synapses.addSynapses(gid, std::move(synapseSpheres));
        }
    }
};

struct EfferentLoader
{
    static void load(const bbploader::LoadContext &context, brayns::Model &model)
    {
        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        auto &synapses = model.addComponent<SynapseComponent>();

        std::map<uint32_t, std::vector<brayns::Sphere>> synapseGeometry;
        const brain::Synapses synapseData (circuit.getEfferentSynapses(gids));
        for(const auto &synapse : synapseData)
        {
            const auto position = synapse.getPresynapticSurfacePosition();
            const auto gid = synapse.getPresynapticGID();
            auto &buffer = synapseGeometry[gid];
            buffer.push_back({position, 2.f});
        }

        for(auto &[gid, synapseSpheres] : synapseGeometry)
        {
            synapses.addSynapses(gid, std::move(synapseSpheres));
        }
    }
};

struct SynapseColorComponentFactory
{
    static void create(const bbploader::LoadContext &context, brayns::Model &model)
    {
        const auto &circuit = context.circuit;
        const auto &config = context.config;

        auto &synapses = model.getComponent<SynapseComponent>();
        auto path = circuit.getSource().getPath();
        auto population = config.getCircuitPopulation();
        auto colorData = std::make_unique<bbploader::BBPSynapseColorData>(std::move(path), std::move(population));
        auto colorHandler = std::make_unique<SynapseColorHandler>(synapses);
        model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));
    }
};
}

namespace bbploader
{
void SynapseLoader::load(const LoadContext &context, const bool post, brayns::Model &model)
{
    if (post)
    {
        AfferentLoader::load(context, model);
    }
    else
    {
        EfferentLoader::load(context, model);
    }

    SynapseColorComponentFactory::create(context, model);
}
} // namespace bbploader
