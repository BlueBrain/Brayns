#pragma once

#include <brayns/engine/Model.h>
#include <plugin/api/CircuitColorData.h>
#include <plugin/api/CircuitColorHandler.h>
#include <plugin/io/morphology/neuron/NeuronGeometryMapping.h>

class PrimitiveColorHandler final : public CircuitColorHandler
{
public:
    PrimitiveColorHandler(CircuitColorData &colorData, brayns::Model &model);

    /**
     * @brief updateColorById Updates color of the elements by the ID they are
     * identified by. Specific ids might be specified to isolate the update
     */
    void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    /**
     * @brief updateSingleColor Updates the color of all the elements to the
     * given color
     */
    void updateSingleColor(const brayns::Vector4f &color) override;

    /**
     * @brief updateColor Updates the circuit color according to the given
     * method. If one or more variables are specified, only these will be
     * updated. Otherwise, updates the whole circuit.
     */
    void updateColor(const std::string &method, const std::vector<ColoringInformation> &vars) override;

    /**
     * @brief addMappingForElement adds a new element geometry mapping to the handler
     * @param id
     * @param ranges
     */
    void addMappingForElement(uint64_t id, std::vector<NeuronSectionRange> ranges);

private:
    CircuitColorData &_colorData;
    brayns::Model &_model;
    std::vector<uint64_t> _ids;
    std::vector<std::vector<NeuronSectionRange>> _elementMappings;

};
