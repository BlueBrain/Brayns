/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ModelInfo.h"

namespace
{
class Extractor
{
public:
    template<typename T>
    static const T *extract(const brayns::Model &model)
    {
        auto &components = model.getComponents();
        return components.find<T>();
    }
};
}

namespace brayns
{
ModelInfo::ModelInfo(const Model &model):
    _loadInfo(Extractor::extract<LoadInfo>(model)),
    _metadata(Extractor::extract<Metadata>(model)),
    _baseTransform(Extractor::extract<Transform>(model))
{
}

const LoadInfo *ModelInfo::getLoadInfo() const noexcept
{
    return _loadInfo;
}

const Metadata *ModelInfo::getMetadata() const noexcept
{
    return _metadata;
}

const Transform *ModelInfo::getBaseTransform() const noexcept
{
    return _baseTransform;
}
}
