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
#include "PBRTModel.h"
#include "PBRTScene.h"
#include "util/Util.h"

#include <brayns/common/light/Light.h>
#include <brayns/engine/Model.h>

#include <pbrtv2/accelerators/bvh.h>

#include <pbrtv2/core/light.h>
#include <pbrtv2/core/paramset.h>
#include <pbrtv2/core/volume.h>

#include <pbrtv2/lights/distant.h>
#include <pbrtv2/lights/diffuse.h>
#include <pbrtv2/lights/infinite.h>
#include <pbrtv2/lights/spot.h>

#include <pbrtv2/materials/matte.h>

#include <pbrtv2/shapes/sphere.h>
#include <pbrtv2/shapes/trianglemesh.h>

#include <pbrtv2/textures/constant.h>

namespace brayns
{

pbrt::Shape* CreateSphere(const float radius, const Vector3f& center,
                                          std::vector<std::unique_ptr<pbrt::Transform>>& pool)
{
    pbrt::ParamSet params;

    std::unique_ptr<pbrt::Transform> otwS (new pbrt::Transform(pbrtTranslation(center)));
    std::unique_ptr<pbrt::Transform> wtoS (new pbrt::Transform(otwS->GetInverseMatrix(), otwS->GetMatrix()));

    auto otwFinalPtr = otwS.get();
    auto wtoFinalPtr = wtoS.get();

    pool.push_back(std::move(otwS));
    pool.push_back(std::move(wtoS));

    params.AddFloat("radius", &radius, 1);

    return pbrt::CreateSphereShape(otwFinalPtr, wtoFinalPtr, false, params);
}

PBRTScene::PBRTScene(AnimationParameters& animationParameters,
                     GeometryParameters& geometryParameters,
                     VolumeParameters& volumeParameters)
    : Scene(animationParameters, geometryParameters, volumeParameters)
{
    _backgroundMaterial = std::make_shared<PBRTMaterial>();
}

PBRTScene::~PBRTScene()
{
}

void PBRTScene::commit()
{
    Scene::commit();

    // copy the list to avoid locking the mutex
    ModelDescriptors modelDescriptors;
    {
        auto lock = acquireReadAccess();
        modelDescriptors = _modelDescriptors;
    }

    const bool rebuildScene = isModified() || _lightManager.isModified();
    if(!rebuildScene)
    {
        bool doUpdate = false;
        for (auto& modelDescriptor : modelDescriptors)
        {
            auto& model = modelDescriptor->getModel();
            PBRTModel& pbrtModel = static_cast<PBRTModel&>(model);
            if (model.isDirty() || pbrtModel.materialsDirty())
            {
                // need to continue re-adding the models to update the bounding
                // box model to reflect the new model size
                doUpdate = true;
            }
        }
        if (!doUpdate)
            return;
    }

    _needsRender = true;

    // Release current scene
    _pbrtScene.reset(nullptr);

    std::vector<pbrt::Reference<pbrt::Primitive>> allPrims;
    std::vector<pbrt::Sensor*> allSensors;
    std::vector<pbrt::VolumeRegion*> allVolumes;

    for (auto modelDescriptor : modelDescriptors)
    {
        if (!modelDescriptor->getEnabled())
            continue;

        auto& impl = static_cast<PBRTModel&>(modelDescriptor->getModel());
        const auto& transformation = modelDescriptor->getTransformation();

        BRAYNS_DEBUG << "Committing " << modelDescriptor->getName()
                     << std::endl;

        auto prims = impl.commitToPBRT(transformation, allSensors, allVolumes);
        allPrims.insert(allPrims.end(), prims.begin(), prims.end());

        impl.logInformation();
        impl.markInstancesClean();
    }

    BRAYNS_DEBUG << "Committing root models" << std::endl;

    // Create lights
    commitLights();

    // Append light shapes to the primitive list so they are visible
    allPrims.insert(allPrims.end(), _lightShapes.begin(), _lightShapes.end());

     auto bvh = pbrt::CreateBVHAccelerator(allPrims, pbrt::ParamSet());

    // Compute scene bounds with all the objects information
    _computeBounds();

    // Generate the volume region
    pbrt::VolumeRegion* vr = nullptr;
    if(allVolumes.size() == 1)
        vr = allVolumes[0];
    else if(allVolumes.size() > 1)
        vr = new pbrt::AggregateVolume(allVolumes);

    // Create scene with primitives + lights
    _pbrtScene.reset(new pbrt::Scene(bvh, _lights, vr, allSensors));
}

bool PBRTScene::commitLights()
{
    _lights.clear();
    _lightShapes.clear();

    for(const auto& kv : _lightManager.getLights())
    {
        auto baseLight = kv.second;

        // Add L
        float rgb[3] = {static_cast<float>(baseLight->_color.x),
                        static_cast<float>(baseLight->_color.y),
                        static_cast<float>(baseLight->_color.z)};
        pbrt::Spectrum tempL = pbrt::Spectrum::FromRGB(rgb, pbrt::SpectrumType::SPECTRUM_ILLUMINANT);
        float L[3];
        tempL.ToRGB(L);

        switch(baseLight->_type)
        {
        case LightType::DIRECTIONAL:
        {
            pbrt::ParamSet params;
            const auto dirLight = static_cast<DirectionalLight*>(baseLight.get());
            pbrt::Point from(static_cast<float>(-dirLight->_direction.x),
                             static_cast<float>(-dirLight->_direction.y),
                             static_cast<float>(-dirLight->_direction.z));
            params.AddPoint("from", &from, 1);
            pbrt::Point to(0.f, 0.f, 0.f);
            params.AddPoint("to", &to, 1);

            params.AddRGBSpectrum("L", L, 3);
            auto pbrtDirLight = pbrt::CreateDistantLight(pbrt::Transform(), params);
            _lights.push_back(pbrtDirLight);
            break;
        }
        case LightType::SPHERE:
        {
            pbrt::ParamSet params;
            const auto sphereLight = static_cast<SphereLight*>(baseLight.get());
            const auto radius = sphereLight->_radius;
            const auto& center = sphereLight->_position;

            auto sphereShape = CreateSphere(static_cast<float>(radius), center, _transformPool);
            pbrt::Reference<pbrt::Shape> sphereShapeRef(sphereShape);
            const int nsamples = 16;
            params.AddInt("samples", &nsamples, 1);

            params.AddRGBSpectrum("L", L, 3);
            auto pbrtSphereLight = pbrt::CreateDiffuseAreaLight(pbrt::Transform(), params, sphereShapeRef);
            _lights.push_back(pbrtSphereLight);

            constexpr float WHITE_RGB[3] = {1.f, 1.f, 1.f};
            pbrt::Reference<pbrt::Texture<pbrt::Spectrum>> Kd (
                        new pbrt::ConstantTexture<pbrt::Spectrum>(pbrt::Spectrum::FromRGB(WHITE_RGB)));
            pbrt::Reference<pbrt::Texture<float>> sigma (
                        new pbrt::ConstantTexture<float>(0.f));
            pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);

            pbrt::Reference<pbrt::Material> matte(new pbrt::MatteMaterial(Kd, sigma, bumpmap));

            _lightShapes.push_back(pbrt::Reference<pbrt::Primitive>(
                                       new pbrt::GeometricPrimitive(
                                           sphereShapeRef, matte, pbrtSphereLight)));
            break;
        }
        case LightType::QUAD:
        {
            const auto quadLight = static_cast<QuadLight*>(baseLight.get());

            const Vector3f& p0 = quadLight->_position;
            const Vector3f& p1 = quadLight->_edge1;
            const Vector3f& p3 = quadLight->_edge2;
            const Vector3f p2 = p0 + ((p1 - p0) + (p3 - p0));

            std::vector<pbrt::Point> pos =
            {
                pbrt::Point(p0.x, p0.y, p0.z),
                pbrt::Point(p1.x, p1.y, p1.z),
                pbrt::Point(p2.x, p2.y, p2.z),
                pbrt::Point(p3.x, p3.y, p3.z)
            };

            std::vector<int> indices = {0, 1, 2, 2, 3, 0};

            std::vector<float> uvs =
            {
                0.f, 0.f,
                1.f, 0.f,
                1.f, 1.f,
                0.f, 1.f
            };

            pbrt::ParamSet meshParams;
            meshParams.AddInt("indices", indices.data(), 6);
            meshParams.AddPoint("P", pos.data(), 4);
            meshParams.AddFloat("uv", uvs.data(), 8);

            /*
            // Adjust the orientation (normals) to face the center of the scene bounding box
            auto tempNormal = glm::cross(glm::normalize(p1 - p0), glm::normalize(p3 - p0));
            const auto& bbcenter = getBounds().getCenter();
            auto dirVector = Vector3f(bbcenter.x, bbcenter.y, bbcenter.z) - p0;
            const float dotResult = glm::dot(glm::normalize(tempNormal), glm::normalize(dirVector));
            const bool reverse = dotResult < 0.f;
            */

            std::unique_ptr<pbrt::Transform> dummyTransform (new pbrt::Transform());
            auto dtptr = dummyTransform.get();
            _transformPool.push_back(std::move(dummyTransform));
            pbrt::Reference<pbrt::Shape> areaLight (pbrt::CreateTriangleMeshShape(dtptr, dtptr, true, meshParams));

            pbrt::ParamSet params;

            const int nsamples = 16;
            params.AddInt("nsamples", &nsamples, 1);

            float intensityRGB[3] = {static_cast<float>(baseLight->_intensity),
                                     static_cast<float>(baseLight->_intensity),
                                     static_cast<float>(baseLight->_intensity)};
            params.AddRGBSpectrum("scale", intensityRGB, 1);
            params.AddRGBSpectrum("L", L, 3);

            auto pbrtQuadLight = pbrt::CreateDiffuseAreaLight(pbrt::Transform(), params, areaLight);
            _lights.push_back(pbrtQuadLight);

            constexpr float WHITE_RGB[3] = {1.f, 1.f, 1.f};
            pbrt::Reference<pbrt::Texture<pbrt::Spectrum>> Kd (
                        new pbrt::ConstantTexture<pbrt::Spectrum>(pbrt::Spectrum::FromRGB(WHITE_RGB)));
            pbrt::Reference<pbrt::Texture<float>> sigma (
                        new pbrt::ConstantTexture<float>(0.f));
            pbrt::Reference<pbrt::Texture<float>> bumpmap(nullptr);

            pbrt::Reference<pbrt::Material> matte(new pbrt::MatteMaterial(Kd, sigma, bumpmap));

            _lightShapes.push_back(pbrt::Reference<pbrt::Primitive>(
                                       new pbrt::GeometricPrimitive(
                                           areaLight, matte, pbrtQuadLight)));

            break;
        }
        case LightType::SPOTLIGHT:
        {
            const auto spotLight = static_cast<SpotLight*>(baseLight.get());

            auto pbrtTrans = pbrtTranslation(spotLight->_position);

            pbrt::ParamSet params;

            float coneAngle = static_cast<float>(spotLight->_openingAngle);
            params.AddFloat("coneangle", &coneAngle, 1);

            float coneDeltaAngle = static_cast<float>(spotLight->_penumbraAngle);
            params.AddFloat("conedeltaangle", &coneDeltaAngle, 1);

            pbrt::Point to(static_cast<float>(spotLight->_direction.x),
                           static_cast<float>(spotLight->_direction.y),
                           static_cast<float>(spotLight->_direction.z));
            params.AddPoint("to", &to, 1);

            params.AddRGBSpectrum("I", std::move(L), 3);

            auto pbrtSpotLight = pbrt::CreateSpotLight(pbrtTrans, params);
            _lights.push_back(pbrtSpotLight);
            break;
        }
        case LightType::AMBIENT:
        {
            pbrt::ParamSet params;

            const int nsamples = 16;
            params.AddInt("nsamples", &nsamples, 1);

            params.AddRGBSpectrum("L", L, 3);

            auto pbrtAmbientLight = pbrt::CreateInfiniteLight(pbrt::Transform(), params);
            _lights.push_back(pbrtAmbientLight);
            break;
        }
        }
    }

    return true;
}

ModelPtr PBRTScene::createModel() const
{
    return std::unique_ptr<PBRTModel>(new PBRTModel(_animationParameters,
                                                    _volumeParameters));
}
}
