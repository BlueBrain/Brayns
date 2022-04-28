#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/Primitive.h>

#include <api/vasculature/VasculatureSection.h>

#include <ospray/ospray.h>

class VasculatureComponent final : public brayns::Component
{
public:
    VasculatureComponent(
        std::vector<uint64_t> ids,
        std::vector<brayns::Primitive> geometry,
        std::vector<VasculatureSection> sections);

    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    void onStart() override;

    bool commit() override;

    void onDestroyed() override;

    /**
     * @brief getIDs return the ids of the vessels in this vasculature
     * @return
     */
    const std::vector<uint64_t> &getIDs() const noexcept;

    /**
     * @brief setColor sets an unique color for all the cells
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorBySection sets the color on a per-vasculature-section basis
     * @param colormap
     */
    void setColorBySection(const std::vector<std::pair<VasculatureSection, brayns::Vector4f>> &colormap) noexcept;

    /**
     * @brief setColorById sets colors on a per id (per cell) basis, with the argument being a vector with
     * a color for each ID
     * @param colors
     */
    void setColorById(std::vector<brayns::Vector4f> colors) noexcept;

    /**
     * @brief setColorById sets colors on a per-id (per cell) basis
     * @param colors map of id and its corresponding color
     * @param std::vector<uint64_t> list of vasculature node ids that were not affected
     */
    std::vector<uint64_t> setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept;

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping) noexcept;

    /**
     * @brief Get the Geometry object
     *
     * @return brayns::Geometry<brayns::Primitive>
     */
    brayns::Geometry<brayns::Primitive> &getGeometry() noexcept;

private:
    OSPGeometricModel _model = nullptr;
    brayns::Geometry<brayns::Primitive> _geometry;
    std::vector<uint64_t> _ids;
    std::vector<VasculatureSection> _sections;
    std::vector<brayns::Vector4f> _colors;
    bool _colorsDirty{false};
};
