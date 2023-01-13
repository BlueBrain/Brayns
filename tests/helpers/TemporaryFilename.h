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

#pragma once

#include <filesystem>
#include <random>
#include <string>

class TemporaryFilename
{
public:
    static std::string generateFake(size_t len = 10)
    {
        std::string result;
        do
        {
            result = _generate(len);
        } while (_exists(result));

        return "/" + result + "/fake_file";
    }

    static std::string generateValid(size_t len = 10)
    {
        std::string result;
        do
        {
            result = "/tmp/" + _generate(len);
        } while (_exists(result));

        return result;
    }

private:
    inline static const std::string source = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    static bool _exists(const std::string &filename)
    {
        auto path = std::filesystem::path(filename);
        return std::filesystem::exists(path);
    }

    static std::string _generate(size_t len)
    {
        std::random_device seed;
        std::mt19937 engine(seed());
        std::uniform_int_distribution<> random(0, static_cast<int>(source.length()));

        auto result = std::string();
        result.reserve(len);

        for (size_t i = 0; i < len; ++i)
        {
            auto index = random(engine);
            result.push_back(source[index]);
        }

        return result;
    }
};
