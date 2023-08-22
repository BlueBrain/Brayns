/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Bounds.h>

#include <memory>

namespace brayns
{
template<typename OsprayHandle>
class IDataWrapper
{
public:
    virtual ~IDataWrapper() = default;
    virtual void pushTo(OsprayHandle &handle) = 0;
    virtual std::unique_ptr<IDataWrapper<OsprayHandle>> clone() const noexcept = 0;
};

template<typename DataType, typename OsprayHandle, template<typename> typename Traits>
class DataWrapper final : public IDataWrapper<OsprayHandle>
{
public:
    explicit DataWrapper(DataType value):
        data(std::move(value))
    {
    }

    void pushTo(OsprayHandle &handle) override
    {
        Traits<DataType>::updateData(handle, data);
    }

    std::unique_ptr<IDataWrapper<OsprayHandle>> clone() const noexcept override
    {
        return std::make_unique<DataWrapper<DataType, OsprayHandle, Traits>>(data);
    }

    DataType data;
};

template<typename OsprayHandle>
class ISpatialDataWrapper
{
public:
    virtual ~ISpatialDataWrapper() = default;
    virtual void pushTo(OsprayHandle &handle) = 0;
    virtual std::unique_ptr<ISpatialDataWrapper> clone() const noexcept = 0;
    virtual Bounds computeBounds(const TransformMatrix &matrix) const noexcept = 0;
};

template<typename DataType, typename OsprayHandle, template<typename> typename Traits>
class SpatialDataWrapper final : public ISpatialDataWrapper<OsprayHandle>
{
public:
    using Type = std::decay_t<DataType>;

    explicit SpatialDataWrapper(Type value):
        data(std::move(value))
    {
    }

    void pushTo(OsprayHandle &handle) override
    {
        Traits<Type>::updateData(handle, data);
    }

    std::unique_ptr<ISpatialDataWrapper<OsprayHandle>> clone() const noexcept override
    {
        return std::make_unique<SpatialDataWrapper<Type, OsprayHandle, Traits>>(data);
    }

    Bounds computeBounds(const TransformMatrix &matrix) const noexcept override
    {
        return Traits<Type>::computeBounds(matrix, data);
    };

    Type data;
};
}
