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
    // clang-format off
    _props.setProperty(
        {PARAM_ENABLED, true,
         Property::MetaData{"Enable streaming", "Enable/disable streaming"}});
    _props.setProperty(
        {PARAM_ID, std::string(),
         {"Stream ID", "The ID/name of the stream, equivalent to DEFLECT_ID"}});
    _props.setProperty({PARAM_HOSTNAME, std::string(),
                        {"Stream hostname", "Hostname of Deflect server"}});
    _props.setProperty({PARAM_PORT, (int32_t)deflect::Stream::defaultPortNumber,
                        1, 65535, {"Stream port", "Port of Deflect server"}});
    _props.setProperty({PARAM_COMPRESSION, true,
                        {"Use JPEG compression", "Use JPEG compression"}});
    _props.setProperty({PARAM_TOP_DOWN, false,
                        {"Stream image top-down",
                         "Top-down image orientation instead of bottom-up"}});
    _props.setProperty(
        {PARAM_RESIZING, true,
         {"Allow resizing",
          "Allow resizing of framebuffers from EVT_VIEW_SIZE_CHANGED"}});
    _props.setProperty(
        {PARAM_QUALITY, (int32_t)80, 1, 100, {"JPEG quality", "JPEG quality"}});
    _props.setProperty(
        {PARAM_USE_PIXEL_OP, false,
         {"Use per-tile direct streaming", "Use per-tile direct streaming"}});
    _props.setProperty({PARAM_CHROMA_SUBSAMPLING,
                        int32_t(deflect::ChromaSubsampling::YUV444),
                        enumNames<deflect::ChromaSubsampling>(),
                        {"Chroma subsampling",
                         "Chroma subsampling modes: yuv444, yuv422, yuv420"}});
    // clang-format on
}
}
