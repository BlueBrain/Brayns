#include "SynapseLoader.h"

#include <plugin/io/synapse/components/SynapseComponent.h>

#include <brain/synapse.h>
#include <brain/synapses.h>
#include <brain/synapsesIterator.h>
#include <brain/synapsesStream.h>

namespace
{
struct AfferentLoader
{
    static void load(const brain::Circuit &circuit, const brain::GIDSet &gids, SynapseComponent &synapses)
    {
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
    static void load(const brain::Circuit &circuit, const brain::GIDSet &gids, SynapseComponent &synapses)
    {
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
}

namespace bbploader
{
void SynapseLoader::load(const brain::Circuit &circuit, const brain::GIDSet &gids, const bool post, brayns::Model &mod)
{
    auto &synapseComponent = mod.addComponent<SynapseComponent>();
    if (post)
    {
        AfferentLoader::load(circuit, gids, synapseComponent);
    }
    else
    {
        EfferentLoader::load(circuit, gids, synapseComponent);
    }
}
} // namespace bbploader
