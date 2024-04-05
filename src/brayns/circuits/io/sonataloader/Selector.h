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

#include <brayns/circuits/io/SonataLoaderParameters.h>
#include <brayns/circuits/io/sonataloader/data/Config.h>

#include <bbp/sonata/population.h>

namespace sonataloader
{
class PercentageFilter
{
public:
    static bbp::sonata::Selection filter(const bbp::sonata::Selection &selection, float percentage) noexcept;
};

class NodeSelector
{
public:
    static bbp::sonata::Selection select(const Config &config, const SonataNodePopulationParameters &params);
};

class EdgeSelector
{
public:
    static bbp::sonata::Selection select(
        const Config &config,
        const SonataEdgePopulationParameters &params,
        const bbp::sonata::Selection &baseNodes);
};
}
