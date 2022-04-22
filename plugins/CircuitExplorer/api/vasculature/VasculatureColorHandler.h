#pragma once

#include <brayns/engine/Model.h>

#include <api/coloring/IColorHandler.h>
#include <components/VasculatureComponent.h>

class VasculatureColorHandler final : public IColorHandler
{
public:
    VasculatureColorHandler(VasculatureComponent &vasculature);

    /**
     * @brief updateColor
     * @param color
     */
    void updateColor(const brayns::Vector4f &color) override;

    /**
     * @brief updateColorById
     * @param colorMap
     * @return std::vector<uint64_t>
     */
    std::vector<uint64_t> updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    /**
     * @brief updateColorById
     * @param colors
     */
    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    /**
     * @brief updateColorByMethod
     * @param colorData
     * @param method
     * @param vars
     */
    void updateColorByMethod(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars) override;

    /**
     * @brief updateIndexedColor
     * @param color
     * @param indices
     */
    void updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices) override;

private:
    void _colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars);
    void _colorAll(const std::string &method);

private:
    VasculatureComponent &_vasculature;
};
