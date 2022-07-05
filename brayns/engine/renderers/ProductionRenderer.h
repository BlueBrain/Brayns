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
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
class ProductionRenderer final : public Renderer
{
public:
    ProductionRenderer();

    std::string getName() const noexcept final;

    std::unique_ptr<Renderer> clone() const noexcept final;

protected:
    void commitRendererSpecificParams() final;
};

BRAYNS_JSON_ADAPTER_BEGIN(ProductionRenderer)
BRAYNS_JSON_ADAPTER_GETSET(
    "samples_per_pixel",
    getSamplesPerPixel,
    setSamplesPerPixel,
    "Number of samples per pixel",
    Required(false))
BRAYNS_JSON_ADAPTER_GETSET(
    "max_ray_bounces",
    getMaxRayBounces,
    setMaxRayBounces,
    "Max ray bounces per sample",
    Required(false))
BRAYNS_JSON_ADAPTER_GETSET(
    "background_color",
    getBackgroundColor,
    setBackgroundColor,
    "Background color",
    Required(false))
BRAYNS_JSON_ADAPTER_END()
}
