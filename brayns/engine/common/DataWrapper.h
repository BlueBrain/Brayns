/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/Bounds.h>

#include <cassert>
#include <memory>

namespace brayns
{
template<typename OsprayHandle>
class IDataWrapper
{
public:
    virtual ~IDataWrapper() = default;
    virtual void pushTo(OsprayHandle &handle) = 0;
    virtual std::unique_ptr<IDataWrapper> clone() const noexcept = 0;
    virtual Bounds computeBounds(const Matrix4f &matrix) const noexcept = 0;
};

template<typename DataType, typename OsprayHandle, typename UpdateTraits, typename BoundsTraits = void>
class DataWrapper final : public IDataWrapper<OsprayHandle>
{
public:
    DataWrapper(DataType value)
        : data(std::move(value))
    {
    }

    void pushTo(OsprayHandle &handle) override
    {
        UpdateTraits<DataType>::update(handle, data);
    }

    std::unique_ptr<IDataWrapper> clone() const noexcept override
    {
        return std::make_unique<DataWrapper<DataType>>(data);
    }

    Bounds computeBounds(const Matrix4f &matrix) const noexcept override
    {
        if constexpr (std::is_same_v<BoundsTraits, void>)
        {
            (void)matrix;
            assert(false);
            return {};
        }
        else
        {
            return BoundsTraits<DataType>::compute(matrix, data);
        }
    }

    DataType data;
};
}
