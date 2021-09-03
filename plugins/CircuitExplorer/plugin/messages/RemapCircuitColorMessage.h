/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/network/json/Message.h>
#include <brayns/network/json/MessageAdapter.h>

#include <common/types.h>

namespace brayns
{
BRAYNS_ADAPTER_ENUM(CircuitColorScheme, {"None", CircuitColorScheme::none},
                    {"By id", CircuitColorScheme::by_id},
                    {"By type", CircuitColorScheme::by_type},
                    {"By layer", CircuitColorScheme::by_layer},
                    {"By mtype", CircuitColorScheme::by_mtype},
                    {"By etype", CircuitColorScheme::by_etype},
                    {"By target", CircuitColorScheme::by_target},
                    {"Single material", CircuitColorScheme::single_material})
} // namespace brayns

BRAYNS_MESSAGE_BEGIN(RemapCircuitColorParams)
BRAYNS_MESSAGE_ENTRY(size_t, model_id, "The model to remap")
BRAYNS_MESSAGE_ENTRY(CircuitColorScheme, scheme, "New color scheme")
BRAYNS_MESSAGE_END()

BRAYNS_MESSAGE_BEGIN(RemapCircuitColorResult)
BRAYNS_MESSAGE_ENTRY(bool, updated, "Check if the colors of the model changed")
BRAYNS_MESSAGE_END()