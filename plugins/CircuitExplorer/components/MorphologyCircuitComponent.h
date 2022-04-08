#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/Primitive.h>

#include <api/neuron/NeuronGeometry.h>

#include <ospray/ospray.h>

class MorphologyCircuitComponent final : public brayns::Component
{
public:
    struct MorphologyGeometry
    {
        OSPGeometricModel model{nullptr};
        brayns::Geometry<brayns::Primitive> geometry;
        std::vector<NeuronSectionMapping> sections;
    };

public:
    /**
     * @brief getSizeInBytes
     * @return
     */
    size_t getSizeInBytes() const noexcept override;

    /**
     * @brief computeBounds
     * @param transform
     * @return
     */
    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    /**
     * @brief onStart
     */
    void onStart() override;

    /**
     * @brief commit
     * @return
     */
    bool commit() override;

    /**
     * @brief onDestroyed
     */
    void onDestroyed() override;

    /**
     * @brief Sets the morphologies of this component
     * @param id
     * @param primitives
     * @param map
     */
    void setMorphologies(
        std::vector<uint64_t> id,
        std::vector<std::vector<brayns::Primitive>> primitives,
        std::vector<std::vector<NeuronSectionMapping>> map) noexcept;

    /**
     * @brief getIDs return the cell IDs in this circuit
     * @return const std::vector<uint64_t> &
     */
    const std::vector<uint64_t> &getIDs() const noexcept;

    /**
     * @brief setColor sets an unique color for all the cells
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorById sets colors on a per id (per cell) basis, with the argument being a vector with
     * a color for each ID
     * @param colors
     */
    void setColorById(const std::vector<brayns::Vector4f> &colors);

    /**
     * @brief setColorById sets colors on a per-id (per cell) basis
     * @param colors map of id and its corresponding color
     */
    void setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept;

    /**
     * @brief setSectionColor sets colors on a per morphology section basis
     * @param colors pair of neuron section and its corresponding color
     */
    void setColorBySection(const std::vector<std::pair<NeuronSection, brayns::Vector4f>> &sectionColorList) noexcept;

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping);

private:
    std::vector<uint64_t> _ids;
    std::vector<MorphologyGeometry> _morphologies;
    bool _colorsDirty{false};
};
