/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "ArgvParser.h"

#include <cassert>
#include <stdexcept>

namespace
{
class ArgvStream
{
public:
    ArgvStream(int argc, const char **argv)
        : _argc(argc)
        , _argv(argv)
    {
    }

    bool hasCurrent() const
    {
        return _index < _argc;
    }

    bool hasNext() const
    {
        return _index < _argc - 1;
    }

    const char *getCurrent() const
    {
        assert(hasCurrent());
        return _argv[_index];
    }

    const char *getNext() const
    {
        assert(hasNext());
        return _argv[_index + 1];
    }

    void next()
    {
        assert(hasNext());
        ++_index;
    }

private:
    int _index = 1;
    int _argc = 0;
    const char **_argv = nullptr;
};

class ArgvParserHelper
{
public:
    static brayns::Argv parse(int argc, const char **argv)
    {
        brayns::Argv result;
        ArgvStream stream(argc, argv);
        while (stream.hasCurrent())
        {
            _parseKey(stream, result);
            stream.next();
        }
        return result;
    }

private:
    static void _parseKey(ArgvStream &stream, brayns::Argv &argv)
    {
        auto key = stream.getCurrent();
        if (!_isKey(key))
        {
            throw std::runtime_error("Only keyword arguments are supported");
        }
        _parseValues(stream, argv[key]);
    }

    static void _parseValues(ArgvStream &stream, std::vector<std::string> &values)
    {
        while (true)
        {
            if (!stream.hasNext())
            {
                return;
            }
            auto value = stream.getNext();
            if (_isKey(value))
            {
                return;
            }
            values.push_back(value);
            stream.next();
        }
    }

    static bool _isKey(const char *value)
    {
        return value[0] && value[0] == '-' && value[1] == '-';
    }
};
} // namespace

namespace brayns
{
Argv ArgvParser::parse(int argc, const char **argv)
{
    return ArgvParserHelper::parse(argc, argv);
}
} // namespace brayns
