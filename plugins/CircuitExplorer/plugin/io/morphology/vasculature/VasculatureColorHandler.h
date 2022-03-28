#pragma once

#include <brayns/engine/Model.h>
#include <plugin/api/CircuitColorHandler.h>
#include <plugin/io/morphology/vasculature/VasculatureColorData.h>
#include <plugin/io/morphology/vasculature/VasculatureComponent.h>

class VasculatureColorHandler final : public CircuitColorHandler
{
public:
    VasculatureColorHandler(VasculatureColorData &colorData, VasculatureComponent &vasculature);

    /**
     * @brief updateColorById Updates color of the elements by the ID they are identified by. Specific ids might be
     * specified to isolate the update
     */
    void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    /**
     * @brief updateSingleColor Updates the color of all the elements to the given color
     */
    void updateSingleColor(const brayns::Vector4f &color) override;

    /**
     * @brief updateColor Updates the circuit color according to the given method. If one or more variables are
     * specified, only these will be updated. Otherwise, updates the whole vasculature.
     */
    void updateColor(const std::string &method, const std::vector<ColoringInformation> &vars) override;

    /**
     * @brief updateSimulationColor
     * @param color
     * @param indices
     */
    void updateSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices) override;

private:
    void _colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars);
    void _colorAll(const std::string &method);

private:
    VasculatureColorData &_colorData;
    VasculatureComponent &_vasculature;
};
