#pragma once

#include <brayns/engine/Model.h>

#include <api/coloring/IColorHandler.h>
#include <components/VasculatureComponent.h>

class VasculatureColorHandler final : public IColorHandler
{
public:
    VasculatureColorHandler(VasculatureComponent &vasculature);

    void updateColor(const brayns::Vector4f &color) override;

    std::vector<uint64_t> updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    void updateColorByMethod(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars) override;

    void updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices) override;

private:
    void _colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars);
    void _colorAll(const std::string &method);

private:
    VasculatureComponent &_vasculature;
};
