/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "TransferFunctionRendererComponent.h"

#include <brayns/engine/Model.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/TransferFunctionComponent.h>

#include <ospray/SDK/common/OSPCommon.h>
#include <ospray/ospray_cpp/Data.h>

namespace
{
struct TransferFunctionParameters
{
    inline static const std::string color = "color";
    inline static const std::string opacity = "opacity";
    inline static const std::string valueRange = "value";
};
}

namespace brayns
{
TransferFunctionRendererComponent::TransferFunctionRendererComponent()
    : _osprayTransferFunction("piecewiseLinear")
{
}

void TransferFunctionRendererComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<TransferFunctionComponent>();
}

bool TransferFunctionRendererComponent::manualCommit()
{
    Model &model = getModel();
    auto &transferFunction = ExtractModelObject::extractTransferFunction(model);
    if (!transferFunction.isModified())
    {
        return false;
    }

    constexpr auto stride = 4 * sizeof(float);
    auto &colors = transferFunction.getColors();
    auto &color = colors.front();
    auto colorSize = colors.size();
    auto colorData = ospray::cpp::SharedData(&color.x, colorSize, stride);
    auto opacityData = ospray::cpp::SharedData(&color.w, colorSize, stride);
    _osprayTransferFunction.setParam(TransferFunctionParameters::color, colorData);
    _osprayTransferFunction.setParam(TransferFunctionParameters::opacity, opacityData);

    auto &range = transferFunction.getValuesRange();
    auto ospRange = rkcommon::math::range1f(range.x, range.y);
    _osprayTransferFunction.setParam(TransferFunctionParameters::valueRange, ospRange);

    _osprayTransferFunction.commit();
    transferFunction.resetModified();

    return true;
}

const ospray::cpp::TransferFunction &TransferFunctionRendererComponent::getOsprayObject() const noexcept
{
    return _osprayTransferFunction;
}
}
