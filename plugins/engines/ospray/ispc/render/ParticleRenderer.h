/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * Based on OSPRay implementation
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

#include <plugins/engines/ospray/ispc/render/utils/AbstractRenderer.h>

namespace brayns
{
class ParticleRenderer : public AbstractRenderer
{
public:
    ParticleRenderer();

    /**
       Returns the class name as a string
       @return string containing the full name of the class
    */
    std::string toString() const final { return "brayns::ParticleRenderer"; }
    void commit() final;

private:
    ospray::Ref<ospray::Data> _simulationData;
    ospray::uint64 _simulationDataSize;
    ospray::Ref<ospray::Data> _transferFunctionDiffuseData;
    ospray::Ref<ospray::Data> _transferFunctionEmissionData;
    ospray::uint32 _transferFunctionSize;
    int _randomNumber;
};

} // ::brayns
