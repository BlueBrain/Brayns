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

#include <utility>

namespace brayns
{
ToneMapping::ToneMapping()
{
    _handle = ospNewImageOperation("tonemapper");
}

ToneMapping::ToneMapping(ToneMapping &&other)
{
    *this = std::move(other);
}

ToneMapping &ToneMapping::operator=(ToneMapping &&other)
{
    _handle = other._handle;
    _exposure = other._exposure;
    _contrast = other._contrast;
    _shoulder = other._shoulder;
    _midIn = other._midIn;
    _midOut = other._midOut;
    _hdrMax = other._hdrMax;
    _acesColor = other._acesColor;

    other._handle = nullptr;

    return *this;
}

ToneMapping::~ToneMapping()
{
    if (_handle)
    {
        ospRelease(_handle);
    }
}

void ToneMapping::commit()
{
    ospSetParam(_handle, "exposure", OSPDataType::OSP_FLOAT, &_exposure);
    ospSetParam(_handle, "contrast", OSPDataType::OSP_FLOAT, &_contrast);
    ospSetParam(_handle, "shoulder", OSPDataType::OSP_FLOAT, &_shoulder);
    ospSetParam(_handle, "midIn", OSPDataType::OSP_FLOAT, &_midIn);
    ospSetParam(_handle, "midOut", OSPDataType::OSP_FLOAT, &_midOut);
    ospSetParam(_handle, "hdrMax", OSPDataType::OSP_FLOAT, &_hdrMax);
    ospSetParam(_handle, "acesColor", OSPDataType::OSP_BOOL, &_acesColor);
    ospCommit(_handle);
}

OSPImageOperation ToneMapping::handle() const noexcept
{
    return _handle;
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
