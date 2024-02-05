/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *          Nadir Roman <nadir.romanguerrero@epfl.ch>
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

// These material enums need to be kept as non enum classes on a separate file
// So when including them in ispc linked object files the compiler wont
// complaint.
// TODO: Remove during engine refactoring

/**
 * @brief The MaterialShadingMode enum list the possible shading modes
 *        for the CircuitExplorer renderers
 */
enum MaterialShadingMode
{
    none = 0,
    diffuse = 1,
    electron = 2,
    cartoon = 3,
    electron_transparency = 4,
    perlin = 5,
    diffuse_transparency = 6,
    checker = 7
};

/**
 * @brief The MaterialClippingMode enum list the possible type of clipping
 *        that can be applied at the material level on the CircuitExplorer
 *        renderers
 */
enum MaterialClippingMode
{
    no_clipping = 0,
    plane = 1,
    sphere = 2
};
