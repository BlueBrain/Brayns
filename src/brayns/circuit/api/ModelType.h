/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include <string>

struct ModelType
{
    static inline const std::string neurons = "neurons";
    static inline const std::string astroctyes = "astrocytes";
    static inline const std::string vasculature = "vasculature";
    static inline const std::string afferentSynapses = "afferent_synapses";
    static inline const std::string efferentSynapses = "efferent_synapses";
    static inline const std::string endfeet = "endfeet";
    static inline const std::string morphology = "morphology";
};
