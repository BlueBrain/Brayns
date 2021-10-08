/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "DeflectParameters.h"
#include "utils.h"

#include <deflect/Stream.h>

namespace brayns
{
DeflectParameters::DeflectParameters()
    : _props("Deflect plugin parameters")
{
    _props.add({PARAM_ENABLED,
                true,
                {"Enable streaming", "Enable/disable streaming"}});
    _props.add(
        {PARAM_ID,
         std::string(),
         {"Stream ID", "The ID/name of the stream, equivalent to DEFLECT_ID"}});
    _props.add({PARAM_HOSTNAME,
                std::string(),
                {"Stream hostname", "Hostname of Deflect server"}});
    _props.add({PARAM_PORT,
                (int32_t)deflect::Stream::defaultPortNumber,
                {"Stream port", "Port of Deflect server"}});
    _props.add({PARAM_COMPRESSION,
                true,
                {"Use JPEG compression", "Use JPEG compression"}});
    _props.add({PARAM_TOP_DOWN,
                false,
                {"Stream image top-down",
                 "Top-down image orientation instead of bottom-up"}});
    _props.add({PARAM_RESIZING,
                true,
                {"Allow resizing",
                 "Allow resizing of framebuffers from EVT_VIEW_SIZE_CHANGED"}});
    _props.add({PARAM_QUALITY, 80, {"JPEG quality", "JPEG quality"}});
    _props.add(
        {PARAM_USE_PIXEL_OP,
         false,
         {"Use per-tile direct streaming", "Use per-tile direct streaming"}});
    _props.add({PARAM_CHROMA_SUBSAMPLING,
                {int32_t(deflect::ChromaSubsampling::YUV444),
                 enumNames<deflect::ChromaSubsampling>()},
                {"Chroma subsampling",
                 "Chroma subsampling modes: yuv444, yuv422, yuv420"}});
}
} // namespace brayns
