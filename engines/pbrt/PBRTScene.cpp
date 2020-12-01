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

#include "PBRTScene.h"
#include "PBRTMaterial.h"
#include "PBRTModel.h"
#include "util/Util.h"

#include <brayns/common/light/Light.h>
#include <brayns/engine/Model.h>

#include <pbrt/core/light.h>
#include <pbrt/core/paramset.h>
#include <pbrt/lights/diffuse.h>
#include <pbrt/lights/distant.h>
#include <pbrt/lights/infinite.h>
#include <pbrt/lights/spot.h>
#include <pbrt/materials/matte.h>
#include <pbrt/shapes/sphere.h>
#include <pbrt/shapes/triangle.h>
#include <pbrt/textures/constant.h>

namespace brayns
{

std::shared_ptr<pbrt::Shape> CreateSphere(const float radius, const Vector3f& center,
                                          std::vector<std::unique_ptr<pbrt::Transform>>& pool)
{
    pbrt::ParamSet params;

    std::unique_ptr<pbrt::Transform> otwS (new pbrt::Transform(pbrtTranslation(center)));
    std::unique_ptr<pbrt::Transform> wtoS (new pbrt::Transform(otwS->GetInverseMatrix(), otwS->GetMatrix()));

    auto otwFinalPtr = otwS.get();
    auto wtoFinalPtr = wtoS.get();

    pool.push_back(std::move(otwS));
    pool.push_back(std::move(wtoS));

    auto radiusPtr = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
    radiusPtr.get()[0] = radius;
    params.AddFloat("radius", std::move(radiusPtr), 1);

    return pbrt::CreateSphereShape(otwFinalPtr, wtoFinalPtr, false, params);
}

PBRTScene::PBRTScene(AnimationParameters& animationParameters,
                     GeometryParameters& geometryParameters,
                     VolumeParameters& volumeParameters)
    : Scene(animationParameters, geometryParameters, volumeParameters)
    , _currentRenderer(PBRT_INTEGRATOR_DEBUG)
{
    _backgroundMaterial = std::shared_ptr<PBRTMaterial>(new PBRTMaterial());
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

    bool simDirty = false;
    for(auto& modelDescriptor : modelDescriptors)
        simDirty = simDirty || modelDescriptor->getModel().commitSimulationData();

    bool sceneDirty = isModified();
    if(!sceneDirty && !simDirty)
    {
        for (auto& modelDescriptor : modelDescriptors)
        {
            auto& model = modelDescriptor->getModel();
            PBRTModel& pbrtModel = static_cast<PBRTModel&>(model);
            if (model.isDirty() || pbrtModel.materialsDirty())
            {
                // need to continue re-adding the models to update the bounding
                // box model to reflect the new model size
                sceneDirty = true;
                break;
            }
        }
    }

    if(!sceneDirty && !simDirty && !_lightManager.isModified())
        return;

    // Release current scene
    _pbrtScene.reset(nullptr);

    commitLights();

    if(sceneDirty || simDirty)
    {
        std::vector<std::shared_ptr<pbrt::Primitive>> allPrims;
        for (auto modelDescriptor : modelDescriptors)
        {
            if (!modelDescriptor->getEnabled())
                continue;

            auto& impl = static_cast<PBRTModel&>(modelDescriptor->getModel());
            const auto& transformation = modelDescriptor->getTransformation();

            BRAYNS_DEBUG << "Committing " << modelDescriptor->getName()
                         << std::endl;

            // Add model shapes using pbrt specific commit function
            auto prims = impl.commitToPBRT(transformation, _currentRenderer);
            allPrims.insert(allPrims.end(), prims.begin(), prims.end());
            const auto& modelLights = impl.getModelLights();
            _lights.insert(_lights.end(), modelLights.begin(), modelLights.end());

            impl.logInformation();
            impl.markInstancesClean();
        }

        // Add light source shapes so they are visible
        allPrims.insert(allPrims.end(), _lightShapes.begin(), _lightShapes.end());

        BRAYNS_DEBUG << "Committing root models" << std::endl;

       _bvh.reset();
       _bvh = pbrt::CreateBVHAccelerator(allPrims, pbrt::ParamSet());

       // Compute scene bounds with all the objects information
       _computeBounds();
    }

    // Create scene with primitives + lights
    _pbrtScene.reset(new pbrt::Scene(_bvh, _lights));
}

bool PBRTScene::commitLights()
{
    _lights.clear();
    _lightShapes.clear();

    for(const auto& kv : _lightManager.getLights())
    {
        pbrt::ParamSet params;

        auto baseLight = kv.second;

        // Add L
        auto bL = baseLight->_color * baseLight->_intensity;
        pbrt::Float rgb[] = {static_cast<pbrt::Float>(bL.x),
                             static_cast<pbrt::Float>(bL.y),
                             static_cast<pbrt::Float>(bL.z)};
        pbrt::Spectrum tempL = pbrt::Spectrum::FromRGB(rgb, pbrt::SpectrumType::Illuminant);
        std::unique_ptr<pbrt::Float[]> L (new pbrt::Float[3]);
        tempL.ToRGB(L.get());

        switch(baseLight->_type)
        {
        case LightType::DIRECTIONAL:
        {
            const auto dirLight = static_cast<DirectionalLight*>(baseLight.get());
            std::unique_ptr<pbrt::Point3f[]> from (new pbrt::Point3f[1]);
            from.get()[0] = pbrt::Point3f(static_cast<pbrt::Float>(-dirLight->_direction.x),
                                          static_cast<pbrt::Float>(-dirLight->_direction.y),
                                          static_cast<pbrt::Float>(-dirLight->_direction.z));
            params.AddPoint3f("from", std::move(from), 1);
            std::unique_ptr<pbrt::Point3f[]> to (new pbrt::Point3f[1]);
            to.get()[0] = pbrt::Point3f(0.f, 0.f, 0.f);
            params.AddPoint3f("to", std::move(to), 1);

            params.AddRGBSpectrum("L", std::move(L), 3);
            auto pbrtDirLight = pbrt::CreateDistantLight(pbrt::Transform(), params);
            _lights.push_back(pbrtDirLight);
            break;
        }
        case LightType::SPHERE:
        {
            const auto sphereLight = static_cast<SphereLight*>(baseLight.get());
            const auto radius = sphereLight->_radius;
            const auto& center = sphereLight->_position;

            auto sphereShape = CreateSphere(static_cast<float>(radius), center, _transformPool);

            std::unique_ptr<int[]> nsamples (new int[1]);
            nsamples.get()[0] = 8;
            params.AddInt("samples", std::move(nsamples), 1);

            params.AddRGBSpectrum("L", std::move(L), 3);
            auto pbrtSphereLight = pbrt::CreateDiffuseAreaLight(pbrt::Transform(), nullptr, params, sphereShape);
            _lights.push_back(pbrtSphereLight);

            // Create material and shape so the light source is visible in the scene
            constexpr pbrt::Float WHITE_RGB[3] = {1.f, 1.f, 1.f};
            const std::shared_ptr<pbrt::Texture<pbrt::Spectrum>> Kd =
                    std::make_shared<pbrt::ConstantTexture<pbrt::Spectrum>>
                        (pbrt::Spectrum::FromRGB(WHITE_RGB));

            const std::shared_ptr<pbrt::Texture<pbrt::Float>> Sigma =
                    std::make_shared<pbrt::ConstantTexture<pbrt::Float>>(0.f);

            const std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap {nullptr};

            std::shared_ptr<pbrt::Material> pbrtMaterial = std::make_shared<pbrt::MatteMaterial>(
                        Kd, Sigma, bumpmap);

            const pbrt::MediumInterface dummyMI;

            _lightShapes.push_back(std::make_shared<pbrt::GeometricPrimitive>(sphereShape,
                                   pbrtMaterial,
                                   pbrtSphereLight,
                                   dummyMI));

            break;
        }
        case LightType::QUAD:
        {
            const auto quadLight = static_cast<QuadLight*>(baseLight.get());

            const Vector3f& p0 = quadLight->_position;
            const Vector3f& p1 = quadLight->_edge1;
            const Vector3f& p3 = quadLight->_edge2;
            const Vector3f p2 = p0 + ((p1 - p0) + (p3 - p0));

            std::unique_ptr<pbrt::Point3f[]> pos (new pbrt::Point3f[4]);
            pos.get()[0] = pbrt::Point3f(p0.x, p0.y, p0.z);
            pos.get()[1] = pbrt::Point3f(p1.x, p1.y, p1.z);
            pos.get()[2] = pbrt::Point3f(p2.x, p2.y, p2.z);
            pos.get()[3] = pbrt::Point3f(p3.x, p3.y, p3.z);

            std::unique_ptr<int[]> indices (new int[6]);
            indices.get()[0] = 0;
            indices.get()[1] = 1;
            indices.get()[2] = 2;
            indices.get()[3] = 2;
            indices.get()[4] = 3;
            indices.get()[5] = 0;

            std::unique_ptr<pbrt::Point2f[]> uvs (new pbrt::Point2f[4]);
            uvs.get()[0] = pbrt::Point2f(0.f, 0.f);
            uvs.get()[0] = pbrt::Point2f(1.f, 0.f);
            uvs.get()[0] = pbrt::Point2f(1.f, 1.f);
            uvs.get()[0] = pbrt::Point2f(0.f, 1.f);

            pbrt::ParamSet meshParams;
            meshParams.AddInt("indices", std::move(indices), 6);
            meshParams.AddPoint3f("P", std::move(pos), 4);
            meshParams.AddPoint2f("uv", std::move(uvs), 4);

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
            auto areaLightShape = pbrt::CreateTriangleMeshShape(dtptr, dtptr, true, meshParams);

            std::unique_ptr<int[]> nsamples (new int[1]);
            nsamples.get()[0] = 8;
            params.AddInt("samples", std::move(nsamples), 1);

            params.AddRGBSpectrum("L", std::move(L), 3);
            auto pbrtQuadLightA = pbrt::CreateDiffuseAreaLight(pbrt::Transform(), nullptr, params, areaLightShape[0]);
            auto pbrtQuadLightB = pbrt::CreateDiffuseAreaLight(pbrt::Transform(), nullptr, params, areaLightShape[1]);

            _lights.push_back(pbrtQuadLightA);
            _lights.push_back(pbrtQuadLightB);

            // Create material and shape so the light source is visible in the scene
            constexpr pbrt::Float WHITE_RGB[3] = {1.f, 1.f, 1.f};
            const std::shared_ptr<pbrt::Texture<pbrt::Spectrum>> Kd =
                    std::make_shared<pbrt::ConstantTexture<pbrt::Spectrum>>
                        (pbrt::Spectrum::FromRGB(WHITE_RGB));

            const std::shared_ptr<pbrt::Texture<pbrt::Float>> Sigma =
                    std::make_shared<pbrt::ConstantTexture<pbrt::Float>>(0.f);

            const std::shared_ptr<pbrt::Texture<pbrt::Float>> bumpmap {nullptr};

            std::shared_ptr<pbrt::Material> pbrtMaterial = std::make_shared<pbrt::MatteMaterial>(
                        Kd, Sigma, bumpmap);

            const pbrt::MediumInterface dummyMI;

            _lightShapes.push_back(std::make_shared<pbrt::GeometricPrimitive>(areaLightShape[0],
                                   pbrtMaterial,
                                   pbrtQuadLightA,
                                   dummyMI));
            _lightShapes.push_back(std::make_shared<pbrt::GeometricPrimitive>(areaLightShape[1],
                                   pbrtMaterial,
                                   pbrtQuadLightB,
                                   dummyMI));
            break;
        }
        case LightType::SPOTLIGHT:
        {
            const auto spotLight = static_cast<SpotLight*>(baseLight.get());

            auto pbrtTrans = pbrtTranslation(spotLight->_position);

            std::unique_ptr<pbrt::Float[]> coneangle (new pbrt::Float[1]);
            coneangle.get()[0] = static_cast<pbrt::Float>(spotLight->_openingAngle);
            params.AddFloat("coneangle", std::move(coneangle), 1);

            std::unique_ptr<pbrt::Float[]> conedeltaangle (new pbrt::Float[1]);
            conedeltaangle.get()[0] = static_cast<pbrt::Float>(spotLight->_penumbraAngle);
            params.AddFloat("conedeltaangle", std::move(conedeltaangle), 1);

            std::unique_ptr<pbrt::Point3f[]> to (new pbrt::Point3f[1]);
            to.get()[0] = pbrt::Point3f(static_cast<pbrt::Float>(spotLight->_direction.x),
                                        static_cast<pbrt::Float>(spotLight->_direction.y),
                                        static_cast<pbrt::Float>(spotLight->_direction.z));

            params.AddPoint3f("to", std::move(to), 1);

            params.AddRGBSpectrum("I", std::move(L), 3);

            auto pbrtSpotLight = pbrt::CreateSpotLight(pbrtTrans, nullptr, params);
            _lights.push_back(pbrtSpotLight);
            break;
        }
        case LightType::AMBIENT:
        {
            std::unique_ptr<int[]> nsamples (new int[1]);
            nsamples.get()[0] = 16;
            params.AddInt("samples", std::move(nsamples), 1);

            params.AddRGBSpectrum("L", std::move(L), 3);

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
