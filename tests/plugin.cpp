/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#define BOOST_TEST_MODULE braynsPlugin

#include "jsonSerialization.h"

#include "ClientServer.h"

using Vec2 = std::array<unsigned, 2>;
const Vec2 vecVal{{1, 1}};

BOOST_AUTO_TEST_CASE(plugin_actions)
{
    ClientServer clientServer({"--plugin", "myPlugin"});
    makeNotification("hello");
    makeNotification<Vec2>("foo", vecVal);
    BOOST_CHECK_EQUAL(makeRequest<std::string>("who"), "me");
    BOOST_CHECK((makeRequest<Vec2, Vec2>("echo", vecVal) == vecVal));
}
