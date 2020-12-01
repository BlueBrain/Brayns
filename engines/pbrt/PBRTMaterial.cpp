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

#include <pbrt/core/medium.h>
#include <pbrt/core/paramset.h>

#include <pbrt/materials/disney.h>
#include <pbrt/materials/fourier.h>
#include <pbrt/materials/glass.h>
#include <pbrt/materials/kdsubsurface.h>
#include <pbrt/materials/matte.h>
#include <pbrt/materials/metal.h>
#include <pbrt/materials/mirror.h>
#include <pbrt/materials/mixmat.h>
#include <pbrt/materials/plastic.h>
#include <pbrt/materials/substrate.h>
#include <pbrt/materials/subsurface.h>
#include <pbrt/materials/translucent.h>
#include <pbrt/materials/uber.h>

#include <pbrt/textures/constant.h>

namespace brayns
{
// MATERIAL CREATION

/**
 * @brief Creates a constant value texture by using a value stored in the material
 *        property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @param defaultValue The default value to use if the property does not exists
 * @return a shared_ptr with the constant texture
 */
std::shared_ptr<pbrt::ConstantTexture<pbrt::Float>> constFloatTexture(const Material& m,
                                                                     const std::string& prop,
                                                                     const double defaultValue = 0.)
{
    const auto val = static_cast<pbrt::Float>(m.getPropertyOrValue<double>(prop, defaultValue));
    return std::shared_ptr<pbrt::ConstantTexture<pbrt::Float>>
               (new pbrt::ConstantTexture<pbrt::Float>(val));
}

/**
 * @brief Creates a constant value texture by using a value stored in the material
 *        property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @return a shared_ptr with the constant texture, or a null if the property was not present
 */
std::shared_ptr<pbrt::ConstantTexture<pbrt::Float>> constFloatTextureOrNull(const Material& m,
                                                                            const std::string& prop)
{
    if(m.hasProperty(prop))
    {
        const auto val = static_cast<pbrt::Float>(m.getProperty<double>(prop));
        return std::shared_ptr<pbrt::ConstantTexture<pbrt::Float>>
               (new pbrt::ConstantTexture<pbrt::Float>(val));
    }

    return std::shared_ptr<pbrt::ConstantTexture<pbrt::Float>> (nullptr);
}

/**
 * @brief Creates a constant value texture by using a value stored in the material
 *        property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @param defaultValue The default value to use if the property does not exists
 * @return a shared_ptr with the constant texture
 */
std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>> constSpectrumTexture(const Material& m,
                                                                            const std::string& prop,
                                                                            const std::array<double, 3>& defaultValue)
{
    std::array<double, 3> param =
            m.getPropertyOrValue<std::array<double, 3>>(prop, defaultValue);
    pbrt::Float val[] =
    {
        static_cast<pbrt::Float>(param[0]),
        static_cast<pbrt::Float>(param[1]),
        static_cast<pbrt::Float>(param[2])
    };
    return std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>>
               (new pbrt::ConstantTexture<pbrt::Spectrum>
               (pbrt::Spectrum::FromRGB(val)));
}

/**
 * @brief Creates a constant value texture by using a value stored in the material
 *        property map
 * @param m The material which holds the property
 * @param prop The name of the property to check for
 * @return a shared_ptr with the constant texture, or a null texture if the property is not present
 */
std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>> constSpectrumTextureOrNull(const Material& m,
                                                                                  const std::string& prop)
{
    if(m.hasProperty(prop))
    {
        std::array<double, 3> param =
                m.getProperty<std::array<double, 3>>(prop);
        pbrt::Float val[] =
        {
            static_cast<pbrt::Float>(param[0]),
            static_cast<pbrt::Float>(param[1]),
            static_cast<pbrt::Float>(param[2])
        };
        return std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>>
                   (new pbrt::ConstantTexture<pbrt::Spectrum>
                   (pbrt::Spectrum::FromRGB(val)));
    }

    return std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>> (nullptr);
}

/**
 * @brief Creates a constant value texture by using a value stored in the material
 *        property map
 * @param m The material which holds the property
 * @param type The texture type to use to gather the texture data
 * @param defaultValue The default value to use if the property does not exists
 * @return a shared_ptr with the spectrum texture
 */
std::shared_ptr<pbrt::Texture<pbrt::Spectrum>> braynsTextureToPbrtTexture(const Material& m,
                                                                          const TextureType type,
                                                                          const std::array<float, 3>& defaultValue)
{
    std::shared_ptr<pbrt::Texture<pbrt::Spectrum>> result;
    if(m.hasTexture(type))
    {
        auto texPtr = m.getTexture(type);
        result = std::shared_ptr<PBRTTextureImpl<pbrt::Spectrum>>
                    (new PBRTTextureImpl<pbrt::Spectrum>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                         *texPtr));
    }
    else // Otherwise, create a texture with a constant diffuse color
    {
        const pbrt::Float rgbValues[] =
        {
          static_cast<pbrt::Float>(defaultValue[0]),
          static_cast<pbrt::Float>(defaultValue[1]),
          static_cast<pbrt::Float>(defaultValue[2])
        };

        result = std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>>
                    (new pbrt::ConstantTexture<pbrt::Spectrum>
                    (pbrt::Spectrum::FromRGB(rgbValues)));
    }

    return result;
}

/**
 * @brief Creates a constant value texture by using a value stored in the material
 *        property map
 * @param m The material which holds the property
 * @param type The texture type to use to gather the texture data
 * @return a shared_ptr with the spectrum texture, or null texture if the texture type is not present
 */
std::shared_ptr<pbrt::Texture<pbrt::Spectrum>> braynsTextureToPbrtTextureOrNull(const Material& m,
                                                                                const TextureType type)
{
    if(m.hasTexture(type))
    {
        auto texPtr = m.getTexture(type);
        if(texPtr != nullptr) // If we have color texture, use it
                return std::shared_ptr<PBRTTextureImpl<pbrt::Spectrum>>
                          (new PBRTTextureImpl<pbrt::Spectrum>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                                 *texPtr));
    }

    return std::shared_ptr<pbrt::ConstantTexture<pbrt::Spectrum>> (nullptr);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::DisneyMaterial>() const
{
    // COLOR TEXTURE
    auto color = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                            {static_cast<float>(getDiffuseColor().r),
                                             static_cast<float>(getDiffuseColor().g),
                                             static_cast<float>(getDiffuseColor().b)});

    // METALLIC TEXTURE
    auto metallic = constFloatTexture(*this, "disneyMetallic", 0.);

    // ETA TEXTURE
    auto eta = constFloatTexture(*this, "disneyEta", 1.5);

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "disneyRoughness", .5);

    // SPEUCLAR TINT TEXTURE
    auto speculartint = constFloatTexture(*this, "disneySpeculartint", 0.);

    // ANISOTROPIC TEXTURE
    auto anisotropic = constFloatTexture(*this, "disneyAnisotropic", 0.);

    // SHEEN TEXTURE
    auto sheen = constFloatTexture(*this, "disneySheen", 0.);

    // SHEEN TINT TEXTURE
    auto sheentint = constFloatTexture(*this, "disneySheentint", 0.5);

    // CLEARCOAT TEXTURE
    auto clearcoat = constFloatTexture(*this, "disneyClearcoat", 0.);

    // CLEARCOAT GLOSS
    auto clearcoatgloss = constFloatTexture(*this, "disneyClearcoatgloss", 1.);

    // SPECTRANS TEXTURE
    auto spectrans = constFloatTexture(*this, "disneySpectrans", 0.);

    // SCATTER DISTANCE TEXTURES
    auto scatterdistance = constSpectrumTexture(*this, "disneyScatterdistance", {0., 0., 0.});

    // THIN FLAG
    auto thin = getPropertyOrValue<bool>("disneyThin", false);

    // FLATNESS TEXTURE
    auto flatness = constFloatTexture(*this, "disneyFlatness", 0.);

    // DIFFTRANS TEXTURE
    auto difftrans = constFloatTexture(*this, "disneyDifftrans", 1.);

    // BUMP MAP TEXTURE (If there is no bumpmap texture, we must pass null)
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    return new pbrt::DisneyMaterial(color, metallic, eta, roughness, speculartint, anisotropic,
                                    sheen, sheentint, clearcoat, clearcoatgloss, spectrans,
                                    scatterdistance, thin, flatness, difftrans, bumpmap);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::FourierMaterial>() const
{
    auto bsdffile = getPropertyOrValue<std::string>("fourierBsdffile", "");

    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    return new pbrt::FourierMaterial(bsdffile, bumpmap);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::GlassMaterial>() const
{
    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "glassKr", {1., 1., 1.});

    // KT TEXTURE
    auto Kt = constSpectrumTexture(*this, "glassKt", {1., 1., 1.});

    // ETA TEXTURE
    auto eta = constFloatTexture(*this, "glassEta", 1.5);

    // U-ROUGHNESS TEXTURE
    auto roughu = constFloatTexture(*this, "glassUroughness", 0.);

    // V-ROUGHNESS TEXTURE
    auto roughv = constFloatTexture(*this, "glassVroughness", 0.);

    // BUMP MAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    auto remapRoughness = getPropertyOrValue<bool>("glassRemaproughness", true);

    return new pbrt::GlassMaterial(Kr, Kt, roughu, roughv, eta, bumpmap, remapRoughness);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::KdSubsurfaceMaterial>() const
{
    // KD TEXTURE
    auto Kd = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                        {static_cast<float>(getDiffuseColor().r),
                                         static_cast<float>(getDiffuseColor().g),
                                         static_cast<float>(getDiffuseColor().b)});

    // MFP TEXTURE
    auto mfp = constSpectrumTexture(*this, "kdsubsurfMfp", {1., 1., 1.});

    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "kdsubsurfKr", {1., 1., 1.});

    // KT TEXTURE
    auto Kt = constSpectrumTexture(*this, "kdsubsurfKt", {1., 1., 1.});

    // U-ROUGHNESS TEXTURE
    auto roughu = constFloatTexture(*this, "kdsubsurfUroughness", 0.);

    // V-ROUGHNESS TEXTURE
    auto roughv = constFloatTexture(*this, "kdsubsurfVroughness", 0.);

    // BUMP MAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    // REMAP ROUGHNESS FLAG
    auto remapRoughness = getPropertyOrValue<bool>("kdsubsurfRemaproughness", true);

    // ETA VALUE
    auto eta = static_cast<pbrt::Float>(getPropertyOrValue<double>("kdsubsurfEta", 1.33));

    // SCALE VALUE
    auto scale = static_cast<pbrt::Float>(getPropertyOrValue<double>("kdsubsurfScale", 1.));

    // G VALUE
    auto g = static_cast<pbrt::Float>(getPropertyOrValue<double>("kdsubsurfG", 0.));

    return new pbrt::KdSubsurfaceMaterial(scale, Kd, Kr, Kt, mfp, g, eta, roughu, roughv, bumpmap, remapRoughness);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MatteMaterial>() const
{
    // KD TEXTURE
    auto Kd = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                        {static_cast<float>(getDiffuseColor().r),
                                         static_cast<float>(getDiffuseColor().g),
                                         static_cast<float>(getDiffuseColor().b)});

    // SIGMA TEXTURE
    auto sigma = constFloatTexture(*this, "matteSigma", 0.);

    // BUMP MAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    return new pbrt::MatteMaterial(Kd, sigma, bumpmap);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MetalMaterial>() const
{
    // We must create Metal material through PBRT interface so the
    // copper samples can be used to compute the spectrum to be used
    // (these are hardcoded on the source file src/materials/metal.cpp)

    auto eta = hasProperty("metalEta")
                    ? constSpectrumTexture(*this, "metalEta", {1., 1., 1.})
                    : std::shared_ptr<pbrt::Texture<pbrt::Spectrum>>(nullptr);

    auto k = hasProperty("k")
                    ? constSpectrumTexture(*this, "metalK", {1., 1., 1.})
                    : std::shared_ptr<pbrt::Texture<pbrt::Spectrum>>(nullptr);

    auto roughness = constFloatTexture(*this, "metalRoughness", 0.01);

    auto uroughness = constFloatTextureOrNull(*this, "metalRroughness");

    auto vroughness = constFloatTextureOrNull(*this, "metalVroughness");

    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    auto remapRoughness = getPropertyOrValue<bool>("metalRemaproughness", true);

    pbrt::ParamSet geoParams;
    std::map<std::string, std::shared_ptr<pbrt::Texture<pbrt::Spectrum>>> sTex;
    std::map<std::string, std::shared_ptr<pbrt::Texture<pbrt::Float>>> fTex;

    if(eta.get() != nullptr)
    {
        geoParams.AddTexture("eta", "eta");
        sTex["eta"] = eta;
    }
    if(k.get() != nullptr)
    {
        geoParams.AddTexture("k", "k");
        sTex["k"] = k;
    }
    geoParams.AddTexture("roughness", "roughness");
    fTex["roughness"] = roughness;
    if(uroughness.get() != nullptr)
    {
        geoParams.AddTexture("uroughness", "uroughness");
        fTex["uroughness"] = uroughness;
    }
    if(vroughness.get() != nullptr)
    {
        geoParams.AddTexture("vroughness", "vroughness");
        fTex["vroughness"] = vroughness;
    }
    if(bumpmap.get() != nullptr)
    {
        geoParams.AddTexture("bumpmap", "bumpmap");
        fTex["bumpmap"] = bumpmap;
    }
    auto remapVal = std::unique_ptr<bool[]>(new bool(1));
    remapVal.get()[0] = remapRoughness;
    geoParams.AddBool("remaproughness", std::move(remapVal), 1);

    pbrt::TextureParams params (geoParams, pbrt::ParamSet(), fTex, sTex);
    return pbrt::CreateMetalMaterial(params);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MirrorMaterial>() const
{
    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "mirrorKr", {0.9, 0.9, 0.9});

    // BUMPMMAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    return new pbrt::MirrorMaterial(Kr, bumpmap);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::MixMaterial>() const
{
    if(!hasProperty("material_1_class") || !hasProperty("material_2_class"))
    {
        BRAYNS_WARN << "PBRTMaterial: Mix material lacks either or both material "
                    << "classes (material_1_class and material_2_class). Falling "
                    << "back to Matte material";
        return _createPBRTMaterial<pbrt::MatteMaterial>();
    }

    const auto mat1Class = static_cast<PBRTMaterialClass>(getProperty<int>("material1Class"));
    const auto mat2Class = static_cast<PBRTMaterialClass>(getProperty<int>("material2Class"));

    // Nested mix materials would introduce infinite recursion
    if(mat1Class == PBRTMaterialClass::MATERIAL_MIXMAT ||
       mat2Class == PBRTMaterialClass::MATERIAL_MIXMAT)
    {
        BRAYNS_WARN << "PBRTMaterial: Mix material is not allowed to create "
                    << "nested mix materials. Falling back to Matte material";
        return _createPBRTMaterial<pbrt::MatteMaterial>();
    }

    std::shared_ptr<pbrt::Material> mat1 (_instantiateMaterial(mat1Class));
    std::shared_ptr<pbrt::Material> mat2 (_instantiateMaterial(mat2Class));

    auto scale = constSpectrumTexture(*this, "mixAmount", {0.5, 0.5, 0.5});

    return new pbrt::MixMaterial(mat1, mat2, scale);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::PlasticMaterial>() const
{
    // KD TEXTURE
    auto Kd = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                        {static_cast<float>(getDiffuseColor().r),
                                         static_cast<float>(getDiffuseColor().g),
                                         static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks = constSpectrumTexture(*this, "plasticKs", {getSpecularColor().r,
                                                         getSpecularColor().g,
                                                         getSpecularColor().b});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "plasticRoughness", 0.01);

    // BUMPMMAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    // REMAP ROUGNESS FLAG
    auto remapRoughness = getPropertyOrValue<bool>("plasticRemaproughness", true);

    return new pbrt::PlasticMaterial(Kd, Ks, roughness, bumpmap, remapRoughness);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::SubstrateMaterial>() const
{
    // KD TEXTURE
    auto Kd = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
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
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    // REMAP ROUGNESS FLAG
    auto remapRoughness = getPropertyOrValue<bool>("substrateRemaproughness", true);

    return new pbrt::SubstrateMaterial(Kd, Ks, uroughness, vroughness, bumpmap, remapRoughness);
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

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::SubsurfaceMaterial>() const
{
    // MEDIUM TYPE NAME
    auto subsurfScattName = getPropertyOrValue<std::string>("subsurfName", "Skin1");
    pbrt::Float sigARGB[3] = {static_cast<pbrt::Float>(0.0011),
                             static_cast<pbrt::Float>(0.0024),
                             static_cast<pbrt::Float>(0.014)};
    pbrt::Float sigSRGB[3] = {static_cast<pbrt::Float>(2.55),
                             static_cast<pbrt::Float>(3.21),
                             static_cast<pbrt::Float>(3.77)};
    pbrt::Spectrum sigA = pbrt::Spectrum::FromRGB(sigARGB);
    pbrt::Spectrum sigS = pbrt::Spectrum::FromRGB(sigSRGB);
    const bool found = pbrt::GetMediumScatteringProperties(subsurfScattName,
                                                           &sigA, &sigS);

    // G VALUE IF MEDIUM TYPE NAME DOES NOT EXISTS
    pbrt::Float g = static_cast<pbrt::Float>(0.0);
    if(!found)
        g = static_cast<pbrt::Float>(getPropertyOrValue<double>("g", 0.));

    // SCALE VALUE
    auto scale = static_cast<pbrt::Float>(getPropertyOrValue<double>("subsurfScale", 1.));

    // ETA VALUE
    auto eta = static_cast<pbrt::Float>(getPropertyOrValue<double>("subsurfEta", 1.33));

    // SIGMA A TEXTURE
    auto sigATexture = constSpectrumTexture(*this, "subsurfSigmaA", {static_cast<double>(sigA[0]),
                                                                       static_cast<double>(sigA[1]),
                                                                       static_cast<double>(sigA[2])});

    // SIGMA S  TEXTURE
    auto sigSTexture = constSpectrumTexture(*this, "subsurfSigmaS", {static_cast<double>(sigS[0]),
                                                                       static_cast<double>(sigS[1]),
                                                                       static_cast<double>(sigS[2])});

    // KR TEXTURE
    auto Kr = constSpectrumTexture(*this, "subsurfKr", {1., 1., 1.});

    // KT TEXTURE
    auto Kt = constSpectrumTexture(*this, "subsurfKt", {1., 1., 1.});

    // U - ROUGHNESS TEXTURE
    auto roughu = constFloatTexture(*this, "subsurfUroughness", 0.);

    // V - ROUGHNESS TEXTURE
    auto roughv = constFloatTexture(*this, "subsurfVroughness", 0.);

    // BUMPMMAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    // REMAP ROUGNESS FLAG
    auto remapRoughness = getPropertyOrValue<bool>("subsurfRemaproughness", true);

    return new pbrt::SubsurfaceMaterial(scale, Kr, Kt, sigATexture, sigSTexture, g,
                                        eta, roughu, roughv, bumpmap, remapRoughness);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::TranslucentMaterial>() const
{
    // KD TEXTURE
    auto Kd = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                        {static_cast<float>(getDiffuseColor().r),
                                         static_cast<float>(getDiffuseColor().g),
                                         static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks = braynsTextureToPbrtTexture(*this, TextureType::specular,
                                        {static_cast<float>(getSpecularColor().r),
                                         static_cast<float>(getSpecularColor().g),
                                         static_cast<float>(getSpecularColor().b)});

    // REFLECT TEXTURE
    auto reflect = braynsTextureToPbrtTexture(*this, TextureType::reflection,
                                        {static_cast<float>(getReflectionIndex()),
                                         static_cast<float>(getReflectionIndex()),
                                         static_cast<float>(getReflectionIndex())});

    // TRANSMIT TEXTURE
    auto transmit = braynsTextureToPbrtTexture(*this, TextureType::refraction,
                                        {static_cast<float>(getRefractionIndex()),
                                         static_cast<float>(getRefractionIndex()),
                                         static_cast<float>(getRefractionIndex())});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "translucentRoughness", 0.1);

    // BUMPMMAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    // REMAP ROUGNESS FLAG
    auto remapRoughness = getPropertyOrValue<bool>("translucentRemaproughness", true);

    return new pbrt::TranslucentMaterial(Kd, Ks, roughness, reflect,
                                         transmit, bumpmap, remapRoughness);
}

template<>
pbrt::Material* PBRTMaterial::_createPBRTMaterial<pbrt::UberMaterial>() const
{
    // KD TEXTURE
    auto Kd = braynsTextureToPbrtTexture(*this, TextureType::diffuse,
                                        {static_cast<float>(getDiffuseColor().r),
                                         static_cast<float>(getDiffuseColor().g),
                                         static_cast<float>(getDiffuseColor().b)});

    // KS TEXTURE
    auto Ks = braynsTextureToPbrtTexture(*this, TextureType::specular,
                                        {static_cast<float>(getSpecularColor().r),
                                         static_cast<float>(getSpecularColor().g),
                                         static_cast<float>(getSpecularColor().b)});

    // KR TEXTURE
    auto Kr = braynsTextureToPbrtTexture(*this, TextureType::reflection,
                                        {static_cast<float>(getReflectionIndex()),
                                         static_cast<float>(getReflectionIndex()),
                                         static_cast<float>(getReflectionIndex())});

    // KT TEXTURE
    auto Kt = braynsTextureToPbrtTexture(*this, TextureType::refraction,
                                        {static_cast<float>(getRefractionIndex()),
                                         static_cast<float>(getRefractionIndex()),
                                         static_cast<float>(getRefractionIndex())});

    // ROUGHNESS TEXTURE
    auto roughness = constFloatTexture(*this, "uberRoughness", 0.1);

    // U-ROUGHNESS TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> uroughness = {nullptr};
    if(hasProperty("uberUroughness"))
        uroughness = constFloatTexture(*this, "uberUroughness", 0.1);

    // V-ROUGHNESS TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> vroughness = {nullptr};
    if(hasProperty("uberVroughness"))
        vroughness = constFloatTexture(*this, "uberVroughness", 0.1);

    // ETA TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> eta {nullptr};
    if(hasProperty("uberEta"))
        eta = constFloatTexture(*this, "uberEta");
    else
        eta = constFloatTexture(*this, "uberIndex", 1.5);

    // OPACITY TEXTURE
    auto opacity = constSpectrumTexture(*this, "uberOpacity", {1., 1., 1.});

    // BUMPMMAP TEXTURE
    std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap (nullptr);
    if(hasTexture(TextureType::bump))
    {
        auto bumpTexPtr = getTexture(TextureType::bump);
        bumpmap = std::shared_ptr<PBRTTextureImpl<pbrt::Float>>
                    (new PBRTTextureImpl<pbrt::Float>(std::unique_ptr<pbrt::UVMapping2D>(),
                                                      *bumpTexPtr));
    }

    // REMAP ROUGNESS FLAG
    auto remapRoughness = getPropertyOrValue<bool>("uberRemaproughness", true);

    return new pbrt::UberMaterial(Kd, Ks, Kr, Kt, roughness, uroughness,
                                  vroughness, opacity, eta, bumpmap, remapRoughness);
}


// MATERIAL CLASS IMPLEMENTATION

PBRTMaterial::PBRTMaterial(const PropertyMap& properties)
 : Material(properties)
{
}

void PBRTMaterial::commit(const std::string& renderer)
{
    // Update new renderer (even if its the same)
    _renderer = renderer;

    // If we cannot commit it, it will result on a crash
    auto matClass = PBRTMaterialClass::MATERIAL_MATTE;
    if(hasProperty("materialClass"))
        matClass = static_cast<PBRTMaterialClass>
                                (getProperty<int>("materialClass"));


    // If attributes are modified we have to recreate it
    if(!isModified())
        return;

    // If the selected material does not match with the current integrator,
    // fall back to safe material class (Plastic)
    if(!_checkMaterialIntegrator(matClass))
    {
        BRAYNS_WARN << "PBRT: material " << materialClassToString(matClass)
                    << " unsupported for integrator "
                    << _renderer << ", switching to matte" << std::endl;

        matClass = PBRTMaterialClass::MATERIAL_MATTE;
    }

    pbrt::Material* rawMat = _instantiateMaterial(matClass);
    _pbrtMat.reset(rawMat);

    resetModified();
}

pbrt::Material* PBRTMaterial::_instantiateMaterial(const PBRTMaterialClass matClass) const
{
    pbrt::Material* creationRes = nullptr;
    switch(matClass)
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
        case PBRTMaterialClass::MATERIAL_DISNEY:
            creationRes = _createPBRTMaterial<pbrt::DisneyMaterial>();
        break;
        case PBRTMaterialClass::MATERIAL_MIRROR:
            creationRes = _createPBRTMaterial<pbrt::MirrorMaterial>();
        break;
        case PBRTMaterialClass::MATERIAL_MIXMAT:
            creationRes = _createPBRTMaterial<pbrt::MixMaterial>();
        break;
        case PBRTMaterialClass::MATERIAL_FOURIER:
            creationRes = _createPBRTMaterial<pbrt::FourierMaterial>();
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
            BRAYNS_WARN << "PBRTMaterial: Unknown class given. Falling back to Matte" << std::endl;
            creationRes = _createPBRTMaterial<pbrt::MatteMaterial>();
    }

    return creationRes;
}

void PBRTMaterial::commit()
{
    commit(_renderer);
}

bool PBRTMaterial::_checkMaterialIntegrator(const PBRTMaterialClass matClass) const
{
    return !((matClass == PBRTMaterialClass::MATERIAL_SUBSURFACE ||
              matClass == PBRTMaterialClass::MATERIAL_KDSUBSURFACE) &&
              _renderer != PBRT_INTEGRATOR_PATHTRACER &&
              _renderer != PBRT_INTEGRATOR_VOLUMETRICPATHTRACER &&
              _renderer != PBRT_INTEGRATOR_DEBUG);
}

}
