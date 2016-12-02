/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <brayns/common/types.h>

namespace brayns
{

struct ShortcutInformation
{
    std::string description;
    std::function< void() > functor;
};

enum class SpecialKey
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class KeyboardHandler
{

public:

    KeyboardHandler();

    void registerKeyboardShortcut(
        const unsigned char key,
        const std::string& description,
        std::function< void() > functor );

    void unregisterKeyboardShortcut( const unsigned char key );

    void handleKeyboardShortcut( const unsigned char key );

    void registerSpecialKey(
        const SpecialKey key,
        const std::string& description,
        std::function< void() > functor );

    void unregisterSpecialKey( const SpecialKey key );

    void handle( const SpecialKey key );

    std::string help();

private:

    std::map< unsigned char, ShortcutInformation > _registeredShortcuts;
    std::map< SpecialKey, ShortcutInformation > _registeredSpecialKeys;

};

}

#endif // KEYBOARDHANDLER_H
