/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <brayns/engine/Renderer.h>

namespace brayns
{
class InteractiveRenderer final : public Renderer
{
public:
    std::string_view getName() const noexcept final;

    /**
     * @brief Sets wether casted shadows should be rendered or not
     */
    void setShadowsEnabled(const bool enabled) noexcept;

    /**
     * @brief Returns wether casted shadows are being rendered or not
     */
    bool getShadowsEnabled() const noexcept;

    /**
     * @brief Sets the number of ambient occlusion samples that the renderer will trace
     */
    void setAmbientOcclusionSamples(const int32_t numSamples) noexcept;

    /**
     * @brief Return the number of ambient occlusion samples that the renderer is tracing
     */
    int32_t getAmbientOcclusionSamples() const noexcept;

    Ptr clone() const noexcept final;

protected:
    std::string_view getOSPHandleName() const noexcept final;

    void commitRendererSpecificParams() final;

private:
    bool _shadowsEnabled {true};
    int32_t _aoSamples {10u};
};

template<>
std::string_view EngineObjectName<InteractiveRenderer>::get();
}
