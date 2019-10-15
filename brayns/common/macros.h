/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef MACROS_H
#define MACROS_H

#define SERIALIZATION_ACCESS(type)            \
    namespace staticjson                      \
    {                                         \
    class ObjectHandler;                      \
    void init(brayns::type*, ObjectHandler*); \
    }

#define SERIALIZATION_FRIEND(type) \
    friend void staticjson::init(type*, staticjson::ObjectHandler*);

#ifdef __GNUC__
#define BRAYNS_UNUSED __attribute__((unused))
#else
#define BRAYNS_UNUSED
#endif

#define BRAYNS_UNUSED_VAR(x) (void)x;

#endif // MACROS_H
