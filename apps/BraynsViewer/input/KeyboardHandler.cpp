/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "KeyboardHandler.h"

#include <brayns/common/Log.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/RenderingParameters.h>
#include <brayns/parameters/VolumeParameters.h>

#include <sstream>

namespace brayns
{
void KeyboardHandler::registerKeyboardShortcut(const unsigned char key,
                                               const std::string& description,
                                               std::function<void()> functor)
{
    if (_registeredShortcuts.find(key) != _registeredShortcuts.end())
    {
        Log::error("{} is already registered.", key);
    }
    else
    {
        ShortcutInformation shortcutInformation = {description, functor};
        _registeredShortcuts[key] = shortcutInformation;
    }
    _buildHelp();
}

void KeyboardHandler::unregisterKeyboardShortcut(const unsigned char key)
{
    auto it = _registeredShortcuts.find(key);
    if (it != _registeredShortcuts.end())
        _registeredShortcuts.erase(it);
    _buildHelp();
}

void KeyboardHandler::handleKeyboardShortcut(const unsigned char key)
{
    auto it = _registeredShortcuts.find(key);
    if (it != _registeredShortcuts.end())
    {
        Log::debug("Processing {}.", (*it).second.description);
        (*it).second.functor();
    }
}

void KeyboardHandler::registerSpecialKey(const SpecialKey key,
                                         const std::string& description,
                                         std::function<void()> functor)
{
    if (_registeredSpecialKeys.find(key) != _registeredSpecialKeys.end())
    {
        Log::error("{} is already registered.", key);
    }
    else
    {
        ShortcutInformation shortcutInformation = {description, functor};
        _registeredSpecialKeys[key] = shortcutInformation;
    }
    _buildHelp();
}

void KeyboardHandler::unregisterSpecialKey(const SpecialKey key)
{
    auto it = _registeredSpecialKeys.find(key);
    if (it != _registeredSpecialKeys.end())
        _registeredSpecialKeys.erase(it);
    _buildHelp();
}

void KeyboardHandler::handle(const SpecialKey key)
{
    auto it = _registeredSpecialKeys.find(key);
    if (it != _registeredSpecialKeys.end())
    {
        Log::debug("Processing {}.", (*it).second.description);
        (*it).second.functor();
    }
}

void KeyboardHandler::_buildHelp()
{
    _helpStrings.clear();

    const auto specialKeyToString = [](const SpecialKey key) {
        switch (key)
        {
        case SpecialKey::RIGHT:
            return "Right";
        case SpecialKey::UP:
            return "Up";
        case SpecialKey::DOWN:
            return "Down";
        case SpecialKey::LEFT:
            return "Left";
        };

        return "INVALID";
    };

    for (const auto& registeredShortcut : _registeredShortcuts)
    {
        std::stringstream ss;
        ss << "'" << registeredShortcut.first << "' "
           << registeredShortcut.second.description;
        _helpStrings.push_back(ss.str());
    }
    for (const auto& registeredShortcut : _registeredSpecialKeys)
    {
        std::stringstream ss;
        ss << "'" << specialKeyToString(registeredShortcut.first) << "' "
           << registeredShortcut.second.description;
        _helpStrings.push_back(ss.str());
    }

} // namespace brayns

const std::vector<std::string>& KeyboardHandler::help() const
{
    return _helpStrings;
}
} // namespace brayns
