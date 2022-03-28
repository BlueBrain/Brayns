#pragma once

#include <brayns/engine/ModelComponents.h>

namespace bbploader
{
class SpikeReportComponent final : public brayns::Component
{
public:
    size_t getSizeInBytes() const noexcept override;
};
}
