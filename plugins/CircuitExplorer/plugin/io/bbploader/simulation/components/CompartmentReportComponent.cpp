
#include "CompartmentReportComponent.h"

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/SizeHelper.h>

#include <plugin/components/CircuitColorComponent.h>
#include <plugin/io/simulation/SimulationFrameTime.h>
#include <plugin/io/util/TransferFunctionUtils.h>

namespace
{
std::vector<SimulationMapping> getMapping(brion::CompartmentReport &report)
{
    const auto &ccounts = report.getCompartmentCounts();
    const auto &offsets = report.getOffsets();

    const auto &gids = report.getGIDs();
    std::vector<SimulationMapping> mapping(gids.size());

#pragma omp parallel for
    for (size_t i = 0; i < gids.size(); ++i)
    {
        const auto &count = ccounts[i];
        const auto &offset = offsets[i];

        mapping[i].globalOffset = offset[0];

        mapping[i].compartments = std::vector<uint16_t>(count.begin(), count.end());

        mapping[i].offsets.resize(offset.size());
        for (size_t j = 0; j < offset.size(); ++j)
            mapping[i].offsets[j] = offset[j] - mapping[i].globalOffset;
    }

    return mapping;
}

brayns::OSPBuffer generateTransferFunctionBuffer(const brayns::TransferFunction &tf)
{
    std::vector<brayns::Vector4f> colors (256);

    const auto &range = tf.getValuesRange();
    const auto chunk = (range.y - range.x) / 256.f;

    for(size_t i = 0; i < 256; ++i)
    {
        const auto value = range.x + chunk * i;
        auto color = tf.getColorForValue(value);
        colors[i] = std::move(color);
    }

    return brayns::DataHandler::copyBuffer(colors, OSPDataType::OSP_VEC4F);
}
}

namespace bbploader
{
CompartmentReportComponent::CompartmentReportComponent(std::unique_ptr<brion::CompartmentReport> report,
                                                       const std::vector<CompartmentStructure> &compartments)
 : _report(std::move(report))
 , _offsets(SimulationMappingGenerator::generate(compartments, getMapping(*_report)))
{
}

size_t CompartmentReportComponent::getSizeInBytes() const noexcept
{
    return sizeof(CompartmentReportComponent)
            + brayns::SizeHelper::vectorSize(_offsets)
            + brayns::SizeHelper::vectorSize(_indices);
}

void CompartmentReportComponent::onStart()
{
    auto &model = getModel();
    auto &tf = brayns::ExtractModelObject::extractTransferFunction(model);
    TransferFunctionUtils::set(tf);
    _colors = generateTransferFunctionBuffer(tf);
    tf.resetModified();
}

void CompartmentReportComponent::onPreRender(const brayns::ParametersManager &parameters)
{
    auto &model = getModel();
    auto &tf = brayns::ExtractModelObject::extractTransferFunction(model);
    if(tf.isModified())
    {
        _colors = generateTransferFunctionBuffer(tf);
    }

    const auto &animationParameters =

    auto &colorComponent = model.getComponent<CircuitColorComponent>();
    auto &handler = colorComponent.getColorHandler();

}
}
