#pragma once

#include <brayns/engine/Model.h>

#include <api/IColorHandler.h>
#include <io/circuit/components/SomaCircuitComponent.h>
#include <io/morphology/neuron/NeuronColorMethods.h>

class SomaColorHandler final : public IColorHandler
{
public:
    SomaColorHandler(SomaCircuitComponent &circuit);

    /**
     * @brief updateColor
     * @param color
     */
    void updateColor(const brayns::Vector4f &color) override;

    /**
     * @brief updateColorById
     * @param colorMap
     */
    void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    /**
     * @brief udpateColorById
     * @param colors
     */
    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    /**
     * @brief updateColorByMethod
     * @param colorData
     * @param method
     * @param vars
     */
    void updateColorByMethod(const IColorData &colorData,
                             const std::string &method,
                             const std::vector<ColoringInformation> &vars) override;

    /**
     * @brief updateIndexedColor
     * @param color
     * @param indices
     */
    void updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices) override;

private:
    void _colorWithInput(const IColorData &colorData,
                         const std::string &method,
                         const std::vector<ColoringInformation> &vars);

    void _colorAll(const IColorData &colorData,
                   const std::string &method);

private:
    SomaCircuitComponent &_circuit;
};
