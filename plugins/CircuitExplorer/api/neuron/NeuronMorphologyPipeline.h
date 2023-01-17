/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "INeuronMorphologyProcessor.h"

#include <io/NeuronMorphologyLoaderParameters.h>

#include <memory>
#include <vector>

class NeuronMorphologyPipeline
{
public:
    using Stages = std::vector<std::unique_ptr<INeuronMorphologyProcessor>>;

public:
    static NeuronMorphologyPipeline fromParameters(const NeuronMorphologyLoaderParameters &parameters);

public:
    explicit NeuronMorphologyPipeline(Stages stages);

    void process(NeuronMorphology &morphology) const;

private:
    Stages _stages;
};
