#include "PrimitiveColorHandler.h"

#include <brayns/engine/defaultcomponents/GeometryRendererComponent.h>
#include <brayns/engine/geometries/Primitive.h>

#include <plugin/api/ColorUtils.h>

namespace
{
brayns::Geometry
}

PrimitiveColorHandler::PrimitiveColorHandler(CircuitColorData &colorData, brayns::Model &model)
 : _colorData(colorData)
 , _model(model)
{
}

void PrimitiveColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    if(!colorMap.empty())
    {
        auto inputIterator = colorMap.begin();
        auto idsIterator = _ids.begin();
        auto elementIterator = _elementMappings.begin();

        while(inputIterator != colorMap.end())
        {
            // Iterate over each entry of the sorted input color map
            auto targetId = inputIterator->first;
            auto &color = inputIterator->second;

            // Find the
            while(idsIterator != _ids.end())
            {
                auto checkId = *idsIterator;
                if(checkId == targetId)
                {
                    break;
                }

                ++idsIterator;
                ++elementIterator;
            }

            if(idsIterator == _ids.end())
            {
                break;
            }
        }
    }
    else
    {

    }
}

void PrimitiveColorHandler::updateSingleColor(const brayns::Vector4f &color)
{

}

void PrimitiveColorHandler::updateColor(const std::string &method, const std::vector<ColoringInformation> &vars)
{

}

void PrimitiveColorHandler::addMappingForElement(uint64_t id, std::vector<NeuronSectionRange> ranges)
{
    _ids.push_back(id);
    _elementMappings.push_back(std::move(ranges));
}
