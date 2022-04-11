#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/TriangleMesh.h>

#include <ospray/ospray.h>

/**
 * @brief The EndfeetComponent class is a endfeet connectivity geometry rendering component in which the endfeet
 * geometries are grouped by the astrocyte to which they are connected to
 */
class EndfeetComponent final : public brayns::Component
{
public:
    struct Endfeet
    {
        OSPGeometricModel model;
        brayns::Geometry<brayns::TriangleMesh> geometry;
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
     * @brief getAstroctyeIds Return the ID of the astroctyes that the endfeet are grouped by
     * @return const std::vector<uint64_t> &
     */
    const std::vector<uint64_t> &getAstroctyeIds() const noexcept;

    /**
     * @brief Adds the given endfeet geometry (grouped by the astroctyte id to which they are connected to) to this
     * component
     *
     * @param endfeetGeometry
     */
    void addEndfeet(std::map<uint64_t, std::vector<brayns::TriangleMesh>> &endfeetGeometry);

    /**
     * @brief setColor Sets all the endfeet to the same specified color
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorById Sets the endfeet color on a per associated-astrocyte-id basis. The input vector must contain
     * an element for each astrocyte ID in this component
     * @param colors
     */
    void setColorById(const std::vector<brayns::Vector4f> &colors);

    /**
     * @brief setColorById Sets the synapse color on ap er associated-astrocyte-id basis.
     * @param colorMap
     */
    void setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap);

private:
    std::vector<uint64_t> _astrocyteIds;
    std::vector<Endfeet> _endFeet;
    bool _colorsDirty{false};
};
