/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "Utils.h"

brayns::Vector3f get_translation(const brayns::Matrix4f& matrix)
{
    return brayns::Vector3f(glm::value_ptr(matrix)[12],
                            glm::value_ptr(matrix)[13],
                            glm::value_ptr(matrix)[14]);
}

brain::Matrix4f glm_to_vmmlib(const brayns::Matrix4f& matrix)
{
    brain::Matrix4f tf;
    memcpy(&tf.array, glm::value_ptr(matrix), sizeof(tf.array));
    return tf;
}

brayns::Matrix4f vmmlib_to_glm(const brain::Matrix4f& matrix)
{
    brayns::Matrix4f tf;
    memcpy(glm::value_ptr(tf), &matrix.array, sizeof(matrix.array));
    return tf;
}

bool inBox(const brayns::Vector3f& point, const brayns::Boxf& box)
{
    const auto min = box.getMin();
    const auto max = box.getMax();
    return (point.x >= min.x && point.y >= min.y && point.z >= min.z &&
            point.x <= max.x && point.y <= max.y && point.z <= max.z);
}
