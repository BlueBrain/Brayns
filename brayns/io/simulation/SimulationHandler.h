/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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

#ifndef SIMULATIONHANDLER_H
#define SIMULATIONHANDLER_H

#include <brayns/common/types.h>

namespace brayns
{
class SimulationHandler
{
public:
    virtual ~SimulationHandler() {}

    virtual void* getFrameData( uint64_t frame ) = 0;

    virtual uint64_t getFrameSize() const = 0;

protected:
    uint64_t _currentFrame;
    uint64_t _frameSize;
    std::vector< float > _frameData;
};

}
#endif // SIMULATIONHANDLER_H
