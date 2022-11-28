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

#include "ToneMapping.h"

namespace
{
struct ToneMappingParameters
{
    inline static const std::string osprayName = "tonemapper";
    inline static const std::string exposure = "exposure";
    inline static const std::string contrast = "contrast";
    inline static const std::string shoulder = "shoulder";
    inline static const std::string midIn = "midIn";
    inline static const std::string midOut = "midOut";
    inline static const std::string hdrMax = "hdrMax";
    inline static const std::string acesColor = "acesColor";
};
}

namespace brayns
{
ospray::cpp::ImageOperation ToneMappingFactory::create(const ToneMapping &data)
{
    auto handle = ospray::cpp::ImageOperation(ToneMappingParameters::osprayName);
    handle.setParam(ToneMappingParameters::exposure, data.exposure);
    handle.setParam(ToneMappingParameters::contrast, data.contrast);
    handle.setParam(ToneMappingParameters::shoulder, data.shoulder);
    handle.setParam(ToneMappingParameters::midIn, data.midIn);
    handle.setParam(ToneMappingParameters::midOut, data.midOut);
    handle.setParam(ToneMappingParameters::hdrMax, data.hdrMax);
    handle.setParam(ToneMappingParameters::acesColor, data.acesColor);
    handle.commit();
    return handle;
}
}
