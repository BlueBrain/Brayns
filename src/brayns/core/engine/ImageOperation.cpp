/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "ImageOperation.h"

namespace
{
using namespace brayns;

void setToneMapperParams(OSPImageOperation handle, const ToneMapperSettings &settings)
{
    setObjectParam(handle, "exposure", settings.exposure);
    setObjectParam(handle, "contrast", settings.contrast);
    setObjectParam(handle, "shoulder", settings.shoulder);
    setObjectParam(handle, "midIn", settings.midIn);
    setObjectParam(handle, "midOut", settings.midOut);
    setObjectParam(handle, "acesColor", settings.acesColor);
}
}

namespace brayns
{
ToneMapper createToneMapper(Device &device, const ToneMapperSettings &settings)
{
    auto handle = ospNewImageOperation("tonemapper");
    auto toneMapper = wrapObjectHandleAs<ToneMapper>(device, handle);

    setToneMapperParams(handle, settings);

    commitObject(handle);

    return toneMapper;
}

void ToneMapper::update(const ToneMapperSettings &settings)
{
    auto handle = getHandle();
    setToneMapperParams(handle, settings);
    commitObject(handle);
}
}
