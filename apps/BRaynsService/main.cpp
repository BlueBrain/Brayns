/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include <brayns/common/log.h>
#include <brayns/Brayns.h>

int main(int argc, const char **argv)
{
    BRAYNS_INFO << "Initializing Service..." << std::endl;
    brayns::Brayns brayns(argc, argv);

    brayns::RenderInput renderInput;
    brayns::RenderOutput renderOutput;

    while( true )
    {
        renderInput.position = brayns::Vector3f(0,0,-1);
        renderInput.target = brayns::Vector3f(0,0,0);
        renderInput.up = brayns::Vector3f(0,1,0);

        brayns.render( renderInput, renderOutput );
    }

    return 0;
}
