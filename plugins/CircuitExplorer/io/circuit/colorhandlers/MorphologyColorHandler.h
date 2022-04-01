#pragma once

#include <brayns/engine/Model.h>

#include <api/IColorHandler.h>
#include <io/morphology/neuron/NeuronColorMethods.h>
#include <io/circuit/components/MorphologyCircuitComponent.h>

/**
 * @brief The MorphologyColorHandler class is the handler interface implementation to update colors on
 * morphology circuits
 */
class MorphologyColorHandler final : public IColorHandler
{
public:
    MorphologyColorHandler(MorphologyCircuitComponent &circuit);

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
     * @brief updateColorById
     * @param colors
     */
    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    /**
     * @brief updateColor
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

    void _colorAll(const IColorData &colorData, const std::string &method);

private:
    MorphologyCircuitComponent &_circuit;
};
