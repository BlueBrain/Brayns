#pragma once

#include <brayns/engine/ModelComponents.h>

#include <plugin/api/CircuitColorData.h>
#include <plugin/api/CircuitColorHandler.h>

class CircuitColorComponent : public brayns::Component
{
public:
    CircuitColorComponent(std::unique_ptr<CircuitColorData> data, std::unique_ptr<CircuitColorHandler> handler);

    CircuitColorData &getColorData() noexcept;
    CircuitColorHandler &getColorHandler() noexcept;

private:
    std::unique_ptr<CircuitColorData> _colorData;
    std::unique_ptr<CircuitColorHandler> _colorHandler;
};
