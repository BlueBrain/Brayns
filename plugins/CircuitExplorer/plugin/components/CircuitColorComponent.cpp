#include "CircuitColorComponent.h"

CircuitColorComponent::CircuitColorComponent(
        std::unique_ptr<CircuitColorData> data, std::unique_ptr<CircuitColorHandler> handler)
 : _colorData(std::move(data))
 , _colorHandler(std::move(handler))
{
}

size_t CircuitColorComponent::getSizeInBytes() const noexcept
{
    return sizeof(CircuitColorComponent);
}

CircuitColorData &CircuitColorComponent::getColorData() noexcept
{
    return *_colorData;
}

CircuitColorHandler &CircuitColorComponent::getColorHandler() noexcept
{
    return *_colorHandler;
}
