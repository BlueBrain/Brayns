#include "BBPSynapseColorHandler.h"

#include <plugin/api/ColorUtils.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

namespace bbploader
{
void BBPSynapseColorHandler::_setElementsImpl(
    const std::vector<uint64_t>& ids,
    std::vector<ElementMaterialMap::Ptr>&& elements)
{
    _gids = ids;
    _synapseMatIds.resize(elements.size());
    for (size_t i = 0; i < elements.size(); ++i)
    {
        const auto& element = elements[i];
        const auto& smm = static_cast<const SynapseMaterialMap&>(*element);
        _synapseMatIds[i].reserve(smm.materials.size());
        for (const auto& mat : smm.materials)
            _synapseMatIds[i].push_back(mat.material);
    }
}

void BBPSynapseColorHandler::_updateColorByIdImpl(
    const std::map<uint64_t, brayns::Vector4f>& colorMap)
{
    if (!colorMap.empty())
    {
        auto it = colorMap.begin();
        auto idIt = _gids.begin();
        size_t index = 0;
        while (it != colorMap.end() && idIt != _gids.end())
        {
            const auto id = it->first;
            if (id > *_gids.rbegin())
                throw std::invalid_argument(
                    "Requested coloring GID '" + std::to_string(id) +
                    "' is beyond the highest GID loaded '" +
                    std::to_string(*_gids.rbegin()) + "'");

            while (id > *idIt && idIt != _gids.end())
            {
                ++idIt;
                ++index;
            }
            if (id == *idIt)
            {
                for (const auto matId : _synapseMatIds[index])
                    _updateMaterial(matId, it->second);
            }
            ++it;
        }
    }
    else
    {
        ColorRoulette r;
        for (const auto& materials : _synapseMatIds)
        {
            const auto& color = r.getNextColor();
            for (const auto mat : materials)
                _updateMaterial(mat, color);
        }
    }
}

void BBPSynapseColorHandler::_updateSingleColorImpl(
    const brayns::Vector4f& color)
{
    for (const auto& materials : _synapseMatIds)
    {
        for (const auto mat : materials)
            _updateMaterial(mat, color);
    }
}
} // namespace bbploader
