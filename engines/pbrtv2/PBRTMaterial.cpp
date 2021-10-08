/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#include "PBRTMaterial.h"
#include "util/PBRTTextureImpl.h"

#include <brayns/common/log.h>

#include <pbrtv2/core/paramset.h>
#include <pbrtv2/core/volume.h>

#include <pbrtv2/materials/glass.h>
#include <pbrtv2/materials/kdsubsurface.h>
#include <pbrtv2/materials/matte.h>
#include <pbrtv2/materials/measured.h>
#include <pbrtv2/materials/metal.h>
#include <pbrtv2/materials/mirror.h>
#include <pbrtv2/materials/mixmat.h>
#include <pbrtv2/materials/plastic.h>
#include <pbrtv2/materials/shinymetal.h>
#include <pbrtv2/materials/substrate.h>
#include <pbrtv2/materials/subsurface.h>
#include <pbrtv2/materials/translucent.h>
#include <pbrtv2/materials/uber.h>

#include <pbrtv2/textures/constant.h>

namespace brayns
{
// MATERIAL CREATION

/**
 * @brief Creates a constant value texture by using a value stored in the
 * material property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @param defaultValue The default value to use if the property does not exists
 * @return a shared_ptr with the constant texture
 */
pbrt::Reference<pbrt::Texture<float>> constFloatTexture(
    const Material& m, const std::string& prop, const double defaultValue = 0.)
{
    const auto val =
        static_cast<float>(m.getPropertyOrValue<double>(prop, defaultValue));
    return pbrt::Reference<pbrt::Texture<float>>(
        new pbrt::ConstantTexture<float>(val));
}

/**
 * @brief Creates a constant value texture by using a value stored in the
 * material property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @return a shared_ptr with the constant texture, or a null if the property was
 * not present
 */
pbrt::Reference<pbrt::Texture<float>> constFloatTextureOrNull(
    const Material& m, const std::string& prop)
{
    if (m.hasProperty(prop))
    {
        const auto val = static_cast<float>(m.getProperty<double>(prop));
        return pbrt::Reference<pbrt::Texture<float>>(
            new pbrt::ConstantTexture<float>(val));
    }

    return pbrt::Reference<pbrt::Texture<float>>(nullptr);
}

/**
 * @brief Creates a constant value texture by using a value stored in the
 * material property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @param defaultValue The default value to use if the property does not exists
 * @return a shared_ptr with the constant texture
 */
pbrt::Reference<pbrt::Texture<pbrt::Spectrum>> constSpectrumTexture(
    const Material& m, const std::string& prop,
    const std::array<double, 3>& defaultValue)
{
    std::array<double, 3> param =
        m.getPropertyOrValue<std::array<double, 3>>(prop, defaultValue);
    float val[] = {static_cast<float>(param[0]), static_cast<float>(param[1]),
                   static_cast<float>(param[2])};
    return pbrt::Reference<pbrt::Texture<pbrt::Spectrum>>(
        new pbrt::ConstantTexture<pbrt::Spectrum>(
            pbrt::Spectrum::FromRGB(val)));
}

/**
 * @brief Creates a constant value texture by using a value stored in the
 * material property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @return a shared_ptr with the constant texture, or a null texture if the
 * property is not present
 */
pbrt::Reference<pbrt::ConstantTexture<pbrt::Spectrum>>
    constSpectrumTextureOrNull(const Material& m, const std::string& prop)
{
    if (m.hasProperty(prop))
    {
        std::array<double, 3> param =
            m.getProperty<std::array<double, 3>>(prop);
        float val[] = {static_cast<float>(param[0]),
                       static_cast<float>(param[1]),
                       static_cast<float>(param[2])};
        return pbrt::Reference<pbrt::ConstantTexture<pbrt::Spectrum>>(
            new pbrt::ConstantTexture<pbrt::Spectrum>(
                pbrt::Spectrum::FromRGB(val)));
    }

    return pbrt::Reference<pbrt::ConstantTexture<pbrt::Spectrum>>(nullptr);
}

/**
 * @brief Creates a constant value texture by using a value stored in the
 * material property map
 * @param m The material which holds the property
 * @param type The texture type to use to gather the texture data
 * @param defaultValue The default value to use if the property does not exists
 * @return a shared_ptr with the spectrum texture
 */
pbrt::Reference<pbrt::Texture<pbrt::Spectrum>> braynsTextureToPbrtTexture(
    const Material& m, const TextureType type,
    const std::array<float, 3>& defaultValue)
{
    pbrt::Texture<pbrt::Spectrum>* rawPtr = nullptr;
    if (m.hasTexture(type))
    {
        auto texPtr = m.getTexture(type);
        rawPtr = new PBRTTextureImpl<pbrt::Spectrum>(new pbrt::UVMapping2D(),
                                                     *texPtr);
    }
    else // Otherwise, create a texture with a constant diffuse color
    {
        const float rgbValues[] = {static_cast<float>(defaultValue[0]),
                                   static_cast<float>(defaultValue[1]),
                                   static_cast<float>(defaultValue[2])};

        rawPtr = new pbrt::ConstantTexture<pbrt::Spectrum>(
            pbrt::Spectrum::FromRGB(rgbValues));
    }

    return pbrt::Reference<pbrt::Texture<pbrt::Spectrum>>(rawPtr);
}

/**
 * @brief Creates a constant value texture by using a value stored in the
 * material property map
 * @param m The material which holds the property
 * @param type The texture type to use to gather the texture data
 * @return a shared_ptr with the spectrum texture, or null texture if the
 * texture type is not present
 */
pbrt::Reference<pbrt::Texture<pbrt::Spectrum>> braynsTextureToPbrtTextureOrNull(
    const Material& m, const TextureType type)
{
    if (m.hasTexture(type))
    {
        auto texPtr = m.getTexture(type);
        if (texPtr != nullptr) // If we have color texture, use it
            return pbrt::Reference<pbrt::Texture<pbrt::Spectrum>>(
                new PBRTTextureImpl<pbrt::Spectrum>(new pbrt::UVMapping2D(),
                                                    *texPtr));
    }

    return pbrt::Reference<pbrt::Texture<pbrt::Spectrum>>(nullptr);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::GlassMaterial>() const
{
    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "glassKr", {1., 1., 1.});

    // KT TEXTURE
    auto Kt = constSpectrumTexture(*this, "glassKt", {1., 1., 1.});

    // INDEX TEXTURE
    auto index = constFloatTexture(*this, "glassIndex", 1.5);

    // BUMP MAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::GlassMaterial(Kr, Kt, index, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::KdSubsurfaceMaterial>()
    const
{
    // KD TEXTURE
    auto Kd =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // MFP TEXTURE
    auto mfp = constFloatTexture(*this, "kdsubsurfMfp", 1.);

    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "kdsubsurfKr", {1., 1., 1.});

    // INDEX TEXTURE
    auto index = constFloatTexture(*this, "kdsubsurfIndex", 1.3);

    // BUMP MAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::KdSubsurfaceMaterial(Kd, Kr, mfp, index, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MatteMaterial>() const
{
    // KD TEXTURE
    auto Kd =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // SIGMA TEXTURE
    auto sigma = constFloatTexture(*this, "matteSigma", 0.);

    // BUMP MAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::MatteMaterial(Kd, sigma, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MeasuredMaterial>()
    const
{
    // BUMP MAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::MeasuredMaterial(
        getPropertyOrValue<std::string>("measuredFilename", ""), bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MetalMaterial>() const
{
    // We must create Metal material through PBRT interface so the
    // copper samples can be used to compute the spectrum to be used
    // (these are hardcoded on the source file src/materials/metal.cpp)

    auto eta = constSpectrumTexture(*this, "metalEta", {1., 1., 1.});

    auto k = constSpectrumTexture(*this, "metalK", {1., 1., 1.});

    auto roughness = constFloatTexture(*this, "metalRoughness", 0.01);

    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::MetalMaterial(eta, k, roughness, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MirrorMaterial>() const
{
    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "mirrorKr", {0.9, 0.9, 0.9});

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::MirrorMaterial(Kr, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MixMaterial>() const
{
    if (!hasProperty("material_1_class") || !hasProperty("material_2_class"))
    {
        BRAYNS_WARN
            << "PBRTMaterial: Mix material lacks either or both material "
            << "classes (material_1_class and material_2_class). Falling "
            << "back to Matte material";
        return _createPBRTMaterial<pbrt::MatteMaterial>();
    }

    const auto mat1Class =
        static_cast<PBRTMaterialClass>(getProperty<int>("material1Class"));
    const auto mat2Class =
        static_cast<PBRTMaterialClass>(getProperty<int>("material2Class"));

    // Nested mix materials would introduce infinite recursion
    if (mat1Class == PBRTMaterialClass::MATERIAL_MIXMAT ||
        mat2Class == PBRTMaterialClass::MATERIAL_MIXMAT)
    {
        BRAYNS_WARN << "PBRTMaterial: Mix material is not allowed to create "
                    << "nested mix materials. Falling back to Matte material";
        return _createPBRTMaterial<pbrt::MatteMaterial>();
    }

    pbrt::Reference<pbrt::Material> mat1(_instantiateMaterial(mat1Class));
    pbrt::Reference<pbrt::Material> mat2(_instantiateMaterial(mat2Class));

    auto scale = constSpectrumTexture(*this, "mixAmount", {0.5, 0.5, 0.5});

    return new pbrt::MixMaterial(mat1, mat2, scale);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::PlasticMaterial>() const
{
    // KD TEXTURE
    auto Kd =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks = constSpectrumTexture(*this, "plasticKs",
                                   {getSpecularColor().r, getSpecularColor().g,
                                    getSpecularColor().b});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "plasticRoughness", 0.01);

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::PlasticMaterial(Kd, Ks, roughness, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::ShinyMetalMaterial>()
    const
{
    // KD TEXTURE
    auto Kr =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks = constSpectrumTexture(*this, "shinymetalKs",
                                   {getSpecularColor().r, getSpecularColor().g,
                                    getSpecularColor().b});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "shinymetalRoughness", 0.01);

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::ShinyMetalMaterial(Ks, roughness, Kr, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::SubstrateMaterial>()
    const
{
    // KD TEXTURE
    auto Kd =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks = constSpectrumTexture(*this, "substrateKs", {0.5, 0.5, 0.5});

    // U-ROUGHNESS TEXTURE
    auto uroughness = constFloatTexture(*this, "substrateUroughness", 0.1);

    // V-ROUGHNESS TEXTURE
    auto vroughness = constFloatTexture(*this, "substrateVroughness", 0.1);

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::SubstrateMaterial(Kd, Ks, uroughness, vroughness, bumpmap);
}

/*
Available medium scattering model names:
 - Apple
 - Chicken1
 - Chicken2
 - Cream
 - Ketchup
 - Marble
 - Potato
 - Skimmilk
 - Skin1
 - Skin2
 - Spectralon
 - Wholemilk
 - Lowfat Milk
 - Reduced Milk
 - Regular Milk
 - Espresso
 - Mint Mocha Coffee
 - Lowfat Soy Milk
 - Regular Soy Milk
 - Lowfat Chocolate Milk
 - Regular Chocolate Milk
 - Coke
 - Pepsi
 - Sprite
 - Gatorade
 - Chardonnay
 - White Zinfandel
 - Merlot
 - Budweiser Beer
 - Coors Light Beer
 - Clorox
 - Apple Juice
 - Cranberry Juice
 - Grape Juice
 - Ruby Grapefruit Juice
 - Shampoo
 - Strawberry Shampoo
 - Lemon  Tea Powder
 - Orange Powder
 - Pink Lemonade Powder
 - Cappuccino Powder
 - Salt Powder
 - Sugar Powder
 - Suisse Mocha Powder
 - Pacific Ocean Surface Water
*/

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::SubsurfaceMaterial>()
    const
{
    // MEDIUM TYPE NAME
    auto subsurfScattName =
        getPropertyOrValue<std::string>("subsurfName", "Skin1");
    float sigARGB[3] = {static_cast<float>(0.0011), static_cast<float>(0.0024),
                        static_cast<float>(0.014)};
    float sigSRGB[3] = {static_cast<float>(2.55), static_cast<float>(3.21),
                        static_cast<float>(3.77)};
    pbrt::Spectrum sigA = pbrt::Spectrum::FromRGB(sigARGB);
    pbrt::Spectrum sigS = pbrt::Spectrum::FromRGB(sigSRGB);
    pbrt::GetVolumeScatteringProperties(subsurfScattName, &sigA, &sigS);

    // SCALE VALUE
    auto scale =
        static_cast<float>(getPropertyOrValue<double>("subsurfScale", 1.));

    // SIGMA A TEXTURE
    sigA.ToRGB(sigARGB);
    auto sigATexture = constSpectrumTexture(*this, "subsurfSigmaA",
                                            {static_cast<double>(sigARGB[0]),
                                             static_cast<double>(sigARGB[1]),
                                             static_cast<double>(sigARGB[2])});

    // SIGMA S  TEXTURE
    sigS.ToRGB(sigSRGB);
    auto sigSTexture = constSpectrumTexture(*this, "subsurfSigmaS",
                                            {static_cast<double>(sigSRGB[0]),
                                             static_cast<double>(sigSRGB[1]),
                                             static_cast<double>(sigSRGB[2])});

    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "subsurfKr", {1., 1., 1.});

    // INDEX TEXTURE
    auto index = constFloatTexture(*this, "subsurfIndex", 1.3);

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::SubsurfaceMaterial(scale, Kr, sigATexture, sigSTexture,
                                        index, bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::TranslucentMaterial>()
    const
{
    // KD TEXTURE
    auto Kd =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks =
        braynsTextureToPbrtTexture(*this, TextureType::specular,
                                   {static_cast<float>(getSpecularColor().r),
                                    static_cast<float>(getSpecularColor().g),
                                    static_cast<float>(getSpecularColor().b)});

    // REFLECT TEXTURE
    auto reflect =
        braynsTextureToPbrtTexture(*this, TextureType::reflection,
                                   {static_cast<float>(getReflectionIndex()),
                                    static_cast<float>(getReflectionIndex()),
                                    static_cast<float>(getReflectionIndex())});

    // TRANSMIT TEXTURE
    auto transmit =
        braynsTextureToPbrtTexture(*this, TextureType::refraction,
                                   {static_cast<float>(getRefractionIndex()),
                                    static_cast<float>(getRefractionIndex()),
                                    static_cast<float>(getRefractionIndex())});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "translucentRoughness", 0.1);

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::TranslucentMaterial(Kd, Ks, roughness, reflect, transmit,
                                         bumpmap);
}

template <>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::UberMaterial>() const
{
    // KD TEXTURE
    auto Kd =
        braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                   {static_cast<float>(getDiffuseColor().r),
                                    static_cast<float>(getDiffuseColor().g),
                                    static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks =
        braynsTextureToPbrtTexture(*this, TextureType::specular,
                                   {static_cast<float>(getSpecularColor().r),
                                    static_cast<float>(getSpecularColor().g),
                                    static_cast<float>(getSpecularColor().b)});

    // KR TEXTURE
    auto Kr =
        braynsTextureToPbrtTexture(*this, TextureType::reflection,
                                   {static_cast<float>(getReflectionIndex()),
                                    static_cast<float>(getReflectionIndex()),
                                    static_cast<float>(getReflectionIndex())});

    // KT TEXTURE
    auto Kt =
        braynsTextureToPbrtTexture(*this, TextureType::refraction,
                                   {static_cast<float>(getRefractionIndex()),
                                    static_cast<float>(getRefractionIndex()),
                                    static_cast<float>(getRefractionIndex())});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "uberRoughness", 0.1);

    // INDEX TEXTURE
    auto index = constFloatTexture(*this, "uberIndex", 1.5);

    // OPACITY TEXTURE
    auto opacity = constSpectrumTexture(*this, "uberOpacity", {1., 1., 1.});

    // BUMPMMAP TEXTURE
    std::shared_ptr<Texture2D> bumpTexPtr{nullptr};
    pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);
    if (hasTexture(TextureType::bump))
    {
        bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = pbrt::Reference<pbrt::Texture<float>>(
            new PBRTTextureImpl<float>(new pbrt::UVMapping2D(), *bumpTexPtr));
    }

    return new pbrt::UberMaterial(Kd, Ks, Kr, Kt, roughness, opacity, index,
                                  bumpmap);
}

// MATERIAL CLASS IMPLEMENTATION

PBRTMaterial::PBRTMaterial(const PropertyMap& properties)
    : Material(properties)
{
}

PBRTMaterial::~PBRTMaterial() {}

pbrt::Material* PBRTMaterial::_instantiateMaterial(
    const PBRTMaterialClass matClass) const
{
    pbrt::Material* creationRes = nullptr;
    switch (matClass)
    {
    case PBRTMaterialClass::MATERIAL_UBER:
        creationRes = _createPBRTMaterial<pbrt::UberMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_GLASS:
        creationRes = _createPBRTMaterial<pbrt::GlassMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_MATTE:
        creationRes = _createPBRTMaterial<pbrt::MatteMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_MEASURED:
        creationRes = _createPBRTMaterial<pbrt::MeasuredMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_MIRROR:
        creationRes = _createPBRTMaterial<pbrt::MirrorMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_MIXMAT:
        creationRes = _createPBRTMaterial<pbrt::MixMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_SHINYMETAL:
        creationRes = _createPBRTMaterial<pbrt::ShinyMetalMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_PLASTIC:
        creationRes = _createPBRTMaterial<pbrt::PlasticMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_SUBSTRATE:
        creationRes = _createPBRTMaterial<pbrt::SubstrateMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_SUBSURFACE:
        creationRes = _createPBRTMaterial<pbrt::SubsurfaceMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_TRANSLUCENT:
        creationRes = _createPBRTMaterial<pbrt::TranslucentMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_KDSUBSURFACE:
        creationRes = _createPBRTMaterial<pbrt::KdSubsurfaceMaterial>();
        break;
    case PBRTMaterialClass::MATERIAL_METAL:
        creationRes = _createPBRTMaterial<pbrt::MetalMaterial>();
        break;
    default:
        BRAYNS_WARN
            << "PBRTMaterial: Unknown class given. Falling back to Matte"
            << std::endl;
        creationRes = _createPBRTMaterial<pbrt::MatteMaterial>();
    }

    return creationRes;
}

void PBRTMaterial::commit()
{
    // If we cannot commit it, it will result on a crash
    auto matClass = PBRTMaterialClass::MATERIAL_MATTE;
    if (hasProperty("materialClass"))
        matClass =
            static_cast<PBRTMaterialClass>(getProperty<int>("materialClass"));

    // If attributes are modified we have to recreate it
    if (!isModified())
        return;

    _pbrtMat = pbrt::Reference<pbrt::Material>(_instantiateMaterial(matClass));

    resetModified();
}

} // namespace brayns
