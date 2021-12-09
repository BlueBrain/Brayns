/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <cassert>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "ImageCodec.h"

namespace brayns
{
/**
 * @brief Helper class to store codecs.
 *
 */
class ImageCodecMap
{
public:
    /**
     * @brief Find a codec for the given format.
     *
     * @param format Image format.
     * @return ImageCodec* Corresponding codec or null if not supported.
     */
    ImageCodec *find(const std::string &format) const
    {
        auto i = _codecs.find(format);
        return i == _codecs.end() ? nullptr : i->second.get();
    }

    /**
     * @brief Add a codec.
     *
     * @param codec Image codec.
     * @throw std::runtime_error Another codec is registered for this format.
     */
    void add(std::unique_ptr<ImageCodec> codec)
    {
        assert(codec);
        auto format = codec->getFormat();
        auto pair = _codecs.emplace(std::move(format), std::move(codec));
        if (!pair.second)
        {
            throw std::runtime_error("Too many codecs for '" + format + "'");
        }
    }

    /**
     * @brief Shortcut to add a codec.
     *
     * @tparam T Codec type.
     * @tparam Args Codec constructor arguments types.
     * @param args Codec constructor arguments.
     */
    template <typename T, typename... Args>
    void add(Args &&... args)
    {
        add(std::make_unique<T>(std::forward<Args>(args)...));
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ImageCodec>> _codecs;
};
} // namespace brayns
