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
#include <brayns/engine/components/TransferFunctionComponent.h>
#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

#include <ospray/ospray_cpp/Data.h>

namespace
{
class TransferFunctionUpdater
{
public:
    static void update(const brayns::TransferFunction &source, const ospray::cpp::TransferFunction &dest)
    {
        static const std::string colorParameter = "color";
        static const std::string opacityParameter = "opacity";
        static const std::string valueRangeParameter = "valueRange";

        auto &colors = source.getColors();
        auto &color = colors.front();
        auto colorSize = colors.size();
        auto &range = source.getValuesRange();
        constexpr auto stride = 4 * sizeof(float);

        dest.setParam(colorParameter, ospray::cpp::SharedData(&color.x, colorSize, stride));
        dest.setParam(opacityParameter, ospray::cpp::SharedData(&color.w, colorSize, stride));
        dest.setParam(valueRangeParameter, range);
    }
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

    TransferFunctionUpdater::update(transferFunction, _osprayTransferFunction);

    _osprayTransferFunction.commit();
    transferFunction.resetModified();

    return true;
}

const ospray::cpp::TransferFunction &TransferFunctionRendererComponent::getOsprayObject() const noexcept
{
    return _osprayTransferFunction;
}
}
