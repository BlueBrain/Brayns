#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/geometries/Sphere.h>

#include <ospray/ospray.h>

/**
 * @brief The SynapseComponent class is a synapse geometry rendering component in which the synapse geometries are
 * grouped by the cell to which they are connected to (not the one they are born from)
 */
class SynapseComponent final : public brayns::Component
{
public:
    struct CellSynapses
    {
        OSPGeometricModel model;
        brayns::Geometry<brayns::Sphere> geometry;
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
     * @brief commit
     * @return
     */
    bool commit() override;

    /**
     * @brief onDestroyed
     */
    void onDestroyed() override;

    /**
     * @brief getCellIds Return the cell ids by which the synapses are grouped
     * @return const std::vector<uint64_t> &
     */
    const std::vector<uint64_t> &getCellIds() const noexcept;

    /**
     * @brief setNumCells Utility functions that allows to optimize memory allocation by letting the component
     * know how many cell and, thus, synapse groups it will have to manage
     * @param size
     */
    void setNumCells(const size_t size) noexcept;

    /**
     * @brief addSynapses Adds a new group of synapses to the component
     * @param cellId
     * @param synapseGeometry
     */
    void addSynapses(uint64_t cellId, std::vector<brayns::Sphere> synapseGeometry);

    /**
     * @brief setColor Sets all the synapses to the same specified color
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorById Sets the synapse color on a per associated-cell-id basis. The input vector must contain
     * an element for each cell ID in this component
     * @param colors
     */
    void setColorById(const std::vector<brayns::Vector4f> &colors);

    /**
     * @brief setColorById Sets the synapse color on ap er associated-cell-id basis.
     * @param colorMap
     */
    void setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap);

private:
    std::vector<uint64_t> _cellIds;
    std::vector<CellSynapses> _synapses;
    bool _colorsDirty {false};
};
