/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "Principled.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct PrincipledParameters
{
    static inline const std::string color = "baseColor";
    static inline const std::string edgeColor = "edgeColor";
    static inline const std::string metallic = "metallic";
    static inline const std::string diffuse = "diffuse";
    static inline const std::string specular = "specular";
    static inline const std::string ior = "ior";
    static inline const std::string transmission = "transmission";
    static inline const std::string transmissionColor = "transmissionColor";
    static inline const std::string transmissionDepth = "transmissionDepth";
    static inline const std::string roughness = "roughness";
    static inline const std::string anisotropy = "anisotropy";
    static inline const std::string rotation = "rotation";
    static inline const std::string thin = "thin";
    static inline const std::string thickness = "thickness";
    static inline const std::string backLight = "backlight";
    static inline const std::string coat = "coat";
    static inline const std::string coatColor = "coatColor";
    static inline const std::string coatThickness = "coatThickness";
    static inline const std::string coatRoughness = "coatRoughness";
    static inline const std::string sheen = "sheen";
    static inline const std::string sheenColor = "sheenColor";
    static inline const std::string sheenTint = "sheenTint";
    static inline const std::string sheenRoughness = "sheenRoughness";
};
}

namespace brayns
{
void MaterialTraits<Principled>::updateData(ospray::cpp::Material &handle, Principled &data)
{
    handle.setParam(PrincipledParameters::color, Vector3f(1.f));

    handle.setParam(PrincipledParameters::edgeColor, data.edgeColor);
    handle.setParam(PrincipledParameters::metallic, data.metallic);
    handle.setParam(PrincipledParameters::diffuse, data.diffuse);
    handle.setParam(PrincipledParameters::specular, data.specular);
    handle.setParam(PrincipledParameters::ior, data.ior);
    handle.setParam(PrincipledParameters::transmission, data.transmission);
    handle.setParam(PrincipledParameters::transmissionColor, data.transmissionColor);
    handle.setParam(PrincipledParameters::transmissionDepth, data.transmissionDepth);
    handle.setParam(PrincipledParameters::roughness, data.roughness);
    handle.setParam(PrincipledParameters::anisotropy, data.anisotropy);
    handle.setParam(PrincipledParameters::rotation, data.anisotropyRotation);
    handle.setParam(PrincipledParameters::thin, data.thin);
    handle.setParam(PrincipledParameters::thickness, data.thickness);
    handle.setParam(PrincipledParameters::backLight, data.backLight);
    handle.setParam(PrincipledParameters::coat, data.coat);
    handle.setParam(PrincipledParameters::coatColor, data.coatColor);
    handle.setParam(PrincipledParameters::coatThickness, data.coatThickness);
    handle.setParam(PrincipledParameters::coatRoughness, data.coatRoughness);
    handle.setParam(PrincipledParameters::sheen, data.sheen);
    handle.setParam(PrincipledParameters::sheenColor, data.sheenColor);
    handle.setParam(PrincipledParameters::sheenTint, data.sheenTint);
    handle.setParam(PrincipledParameters::sheenRoughness, data.sheenRoughness);
}
}
