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

#include "ProteinComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>

namespace brayns
{
ProteinComponent::ProteinComponent(std::vector<Sphere> sphe, std::vector<Vector4f> colors, std::vector<uint8_t> indx)
 : _model(GeometricModelHandler::create())
 , _colors(std::move(colors))
 , _colorIndices(std::move(indx))
{
    _geometry.set(std::move(sphe));
}

uint64_t ProteinComponent::getSizeInBytes() const noexcept
{
    return sizeof(ProteinComponent)
            + _geometry.getSizeInBytes()
            + sizeof(Vector4f) * _colors.size()
            + sizeof(uint8_t) * _colorIndices.size();
}

void ProteinComponent::onStart()
{
    GeometricModelHandler::addToGeometryGroup(_model, getModel());

    auto colorData = DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    auto indexData = DataHandler::shareBuffer(_colorIndices, OSPDataType::OSP_UCHAR);

    GeometricModelHandler::setColorMap(_model, colorData, indexData);
}

bool ProteinComponent::commit()
{
    bool needsCommit = false;

    if(_geometry.commit())
    {
        GeometricModelHandler::setGeometry(_model, _geometry);
        needsCommit = true;
    }

    auto &material = ExtractModelObject::extractMaterial(getModel());
    if(material.commit())
    {
        GeometricModelHandler::setMaterial(_model, material);
        needsCommit = true;
    }

    if(needsCommit)
    {
        GeometricModelHandler::commitModel(_model);
    }

    return needsCommit;
}

void ProteinComponent::onDestroyed()
{
    GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    GeometricModelHandler::destory(_model);
}
}
