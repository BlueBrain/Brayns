#pragma once

#include <brayns/engine/Model.h>
#include <plugin/api/CircuitColorHandler.h>
#include <plugin/io/morphology/neuron/colorhandlers/NeuronColorData.h>
#include <plugin/io/morphology/neuron/components/MorphologyCircuitComponent.h>

class PrimitiveColorHandler final : public CircuitColorHandler
{
public:
    PrimitiveColorHandler(NeuronColorData &colorData, MorphologyCircuitComponent &circuit);

    /**
     * @brief updateColorById Updates color of the elements by the ID they are
     * identified by. Specific ids might be specified to isolate the update
     */
    void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    /**
     * @brief updateColorById
     * @param colors
     */
    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    /**
     * @brief updateSingleColor Updates the color of all the elements to the
     * given color
     */
    void updateSingleColor(const brayns::Vector4f &color) override;

    /**
     * @brief updateColor
     * @param method
     * @param vars
     */
    void updateColor(const std::string &method, const std::vector<ColoringInformation> &vars) override;

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
    NeuronColorData &_colorData;
    MorphologyCircuitComponent &_circuit;
};
