#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>

#include <plugin/io/simulation/SimulationMapping.h>

#include <brion/compartmentReport.h>

namespace bbploader
{
class CompartmentReportComponent final : public brayns::Component
{
public:
    CompartmentReportComponent(std::unique_ptr<brion::CompartmentReport> report,
                               const std::vector<CompartmentStructure> &compartments);

    size_t getSizeInBytes() const noexcept override;

    void onStart() override;

    void onPreRender(const brayns::ParametersManager &parameters) override;

private:
    const std::unique_ptr<brion::CompartmentReport> _report;
    const std::vector<uint64_t> _offsets;

    brayns::OSPBuffer _colors;
    std::vector<uint8_t> _indices;
};
}
