#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/Sphere.h>

#include <plugin/io/morphology/neuron/NeuronSection.h>

#include <ospray/ospray.h>

class SomaCircuitComponent final : public brayns::Component
{
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
     * @brief setNumMorphologies allows to specify in advance the number ofcells this circuit will hold, so that
     * memory can be allocated
     * @param size number of cells
     */
    void setNumCells(const size_t size) noexcept;

    /**
     * @brief addSoma adds a new soma geometry to this circuit
     * @param id
     * @param geometry
     */
    void addSoma(uint64_t id, brayns::Sphere geometry) noexcept;

    /**
     * @brief getIDs return the ID of the cells in this circuit
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
    void setColorById(std::vector<brayns::Vector4f> colors) noexcept;

    /**
     * @brief setColorById sets colors on a per-id (per cell) basis
     * @param colors map of id and its corresponding color
     */
    void setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept;

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping) noexcept;

private:
    OSPGeometricModel _model = nullptr;
    brayns::Geometry<brayns::Sphere> _geometry;
    std::vector<uint64_t> _ids;
    std::vector<brayns::Vector4f> _colors;
    bool _colorsDirty {false};
};
