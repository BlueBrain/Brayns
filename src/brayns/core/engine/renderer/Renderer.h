/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/core/engine/renderer/RendererData.h>
#include <brayns/core/utils/ModifiedFlag.h>

#include "RendererTraits.h"

#include <ospray/ospray_cpp/Renderer.h>

#include <memory>

namespace brayns
{
class Renderer
{
public:
    template<typename T>
    using Traits = RendererTraits<T>;

    template<typename RendererType>
    explicit Renderer(RendererType data)
    {
        set(std::move(data));
    }

    Renderer(Renderer &&) noexcept;
    Renderer &operator=(Renderer &&) noexcept;

    Renderer(const Renderer &other);
    Renderer &operator=(const Renderer &other);

    /**
     * @brief Tries to cast the renderer data to the given type.
     * @tparam RendererType renderer type to cast the data to
     * @return const RendererType*, will be nullptr if the cast fails
     */
    template<typename RendererType>
    const RendererType *as() const noexcept
    {
        if (auto cast = dynamic_cast<const RendererData<RendererType> *>(_data.get()))
        {
            return &cast->data;
        }
        return nullptr;
    }

    /**
     * @brief Sets the renderer type and recreates the OSPRay handle to accomodate it.
     * @tparam RendererType Type of renderer to set
     * @param data Renderer parameters
     */
    template<typename RendererType>
    void set(RendererType data)
    {
        RendererTraits<RendererType>::checkParameters(data);
        _handleName = RendererTraits<RendererType>::handleName;
        _rendererName = RendererTraits<RendererType>::name;
        _handle = ospray::cpp::Renderer(_handleName);
        _data = std::make_unique<RendererData<RendererType>>(std::move(data));
        _data->pushTo(_handle);
        _flag = true;
    }

    /**
     * @brief Calls the underlying OSPRay commit function, if any parameter has been modified, and resets the modified
     * state.
     * @return true If any parameter was modified and thus the commit function was called.
     * @return false If no parameter was modified.
     */
    bool commit();

    /**
     * @brief Returns the number of pixels to be sent by this renderer.
     * @return size_t
     */
    size_t getSamplesPerPixel() const noexcept;

    /**
     * @brief Returns the renderer name.
     * @return const std::string&.
     */
    const std::string &getName() const noexcept;

    /**
     * @brief Returns the OSPRay renderer handle.
     * @return const ospray::cpp::Renderer&.
     */
    const ospray::cpp::Renderer &getHandle() const noexcept;

private:
    std::string _handleName;
    std::string _rendererName;
    ospray::cpp::Renderer _handle;
    std::unique_ptr<IRendererData> _data;
    ModifiedFlag _flag;
};
}
