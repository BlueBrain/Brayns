/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
class ToneMappingParamsUpdater
{
public:
    static void update(const brayns::ToneMapping &toneMapping)
    {
        static const std::string exposureParam = "exposure";
        static const std::string contrastParam = "contrast";
        static const std::string shoulderParam = "shoulder";
        static const std::string midInParam = "midIn";
        static const std::string midOutParam = "midOut";
        static const std::string hdrMaxParam = "hdrMax";
        static const std::string acesColorParam = "acesColor";

        const auto &osprayObject = toneMapping.getOsprayObject();
        osprayObject.setParam(exposureParam, toneMapping.getExposure());
        osprayObject.setParam(contrastParam, toneMapping.getContrast());
        osprayObject.setParam(shoulderParam, toneMapping.getShoulder());
        osprayObject.setParam(midInParam, toneMapping.getMidIn());
        osprayObject.setParam(midOutParam, toneMapping.getMidOut());
        osprayObject.setParam(hdrMaxParam, toneMapping.getMaxHDR());
        osprayObject.setParam(acesColorParam, toneMapping.usesACESColor());
        osprayObject.commit();
    }
};
}

namespace brayns
{
ToneMapping::ToneMapping()
    : ImageOperation("tonemapper")
{
}

void ToneMapping::commit()
{
    ToneMappingParamsUpdater::update(*this);
}

void ToneMapping::setExposure(const float exposure) noexcept
{
    _exposure = exposure;
}

float ToneMapping::getExposure() const noexcept
{
    return _exposure;
}

void ToneMapping::setContrast(const float contrast) noexcept
{
    _contrast = contrast;
}

float ToneMapping::getContrast() const noexcept
{
    return _contrast;
}

void ToneMapping::setShoulder(const float shoulder) noexcept
{
    _shoulder = shoulder;
}

float ToneMapping::getShoulder() const noexcept
{
    return _shoulder;
}

void ToneMapping::setMidIn(const float midin) noexcept
{
    _midIn = midin;
}

float ToneMapping::getMidIn() const noexcept
{
    return _midIn;
}

void ToneMapping::setMidOut(const float midout) noexcept
{
    _midOut = midout;
}

float ToneMapping::getMidOut() const noexcept
{
    return _midOut;
}

void ToneMapping::setMaxHDR(const float maxHDR) noexcept
{
    _hdrMax = maxHDR;
}

float ToneMapping::getMaxHDR() const noexcept
{
    return _hdrMax;
}

void ToneMapping::useACESColor(const bool val) noexcept
{
    _acesColor = val;
}

bool ToneMapping::usesACESColor() const noexcept
{
    return _acesColor;
}
}
