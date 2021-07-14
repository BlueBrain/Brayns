/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/common/Transformation.h>

#include <brayns/network/message/Message.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(TransformationMessage)
BRAYNS_MESSAGE_ENTRY(Vector3d, translation, "Translation XYZ");
BRAYNS_MESSAGE_ENTRY(Vector3d, scale, "Scale XYZ");
BRAYNS_MESSAGE_ENTRY(Quaterniond, rotation, "Rotation XYZW");
BRAYNS_MESSAGE_ENTRY(Vector3d, rotation_center, "Rotation center XYZ");

BoxMessage& operator=(const Transformation& transformation)
{
    translation = transformation.getTranslation();
    scale = transformation.getScale();
    rotation = transformation.getRotation();
    rotation_center = transformation.getRotationCenter();
    return *this;
}

operator Transformation() const
{
    return {translation, scale, rotation, rotation_center};
}

BRAYNS_MESSAGE_END()
} // namespace brayns