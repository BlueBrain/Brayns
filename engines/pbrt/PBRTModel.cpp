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

#include "PBRTModel.h"
#include "PBRTMaterial.h"
#include "util/PBRTCustomCone.h"
#include "util/PBRTSDFGeometryShape.h"
#include "util/Util.h"

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/engine/Material.h>
#include <brayns/parameters/AnimationParameters.h>

#include <pbrt/core/paramset.h>
#include <pbrt/core/primitive.h>
#include <pbrt/core/transform.h>

#include <pbrt/lights/diffuse.h>

#include <pbrt/media/grid.h>
#include <pbrt/media/homogeneous.h>

#include <pbrt/shapes/cylinder.h>
#include <pbrt/shapes/sphere.h>
#include <pbrt/shapes/triangle.h>

namespace brayns
{

namespace
{
pbrt::Medium* HomogeneusMediumFactory(pbrt::Transform*, const PropertyMap& meta)
{
    const auto g = static_cast<pbrt::Float>(meta.getProperty<double>("g", 0.0));
    const auto sigADArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {.0011f, .0024f, .014f});
    const auto sigSDArr = meta.getProperty<std::array<double, 3>>("sig_s",
                                                                  {2.55f, 3.21f, 3.77f});
    const auto density = static_cast<pbrt::Float>(meta.getProperty<double>("density", 1.0));
    const auto scale = static_cast<pbrt::Float>(meta.getProperty<double>("scale", 1.0));
    const pbrt::Float sigARGB[3] = {static_cast<float>(sigADArr[0]),
                                    static_cast<float>(sigADArr[1]),
                                    static_cast<float>(sigADArr[2])};
    const pbrt::Float sigSRGB[3] = {static_cast<float>(sigSDArr[0]),
                                    static_cast<float>(sigSDArr[1]),
                                    static_cast<float>(sigSDArr[2])};

    const auto sigASpec = (pbrt::Spectrum::FromRGB(sigARGB) * scale * density);
    const auto sigSSpec = (pbrt::Spectrum::FromRGB(sigSRGB) * scale * density);

    return new pbrt::HomogeneousMedium(sigASpec, sigSSpec, g);
}

pbrt::Medium* HeterogeneusMediumFactory(pbrt::Transform*, const PropertyMap& meta)
{
    const auto g = static_cast<pbrt::Float>(meta.getProperty<double>("g", 0.0));
    const auto sigADArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {.0011f, .0024f, .014f});
    const auto sigSDArr = meta.getProperty<std::array<double, 3>>("sig_s",
                                                                  {2.55f, 3.21f, 3.77f});
    const auto minDensity = static_cast<pbrt::Float>(meta.getProperty<double>("min_density", 1.0));
    const auto maxDensity = static_cast<pbrt::Float>(meta.getProperty<double>("max_density", 1.0));
    const auto scale = static_cast<pbrt::Float>(meta.getProperty<double>("scale", 1.0));
    const pbrt::Float sigARGB[3] = {static_cast<float>(sigADArr[0]),
                                    static_cast<float>(sigADArr[1]),
                                    static_cast<float>(sigADArr[2])};
    const pbrt::Float sigSRGB[3] = {static_cast<float>(sigSDArr[0]),
                                    static_cast<float>(sigSDArr[1]),
                                    static_cast<float>(sigSDArr[2])};

    const auto density = (minDensity + maxDensity) * 0.5f;

    const auto sigASpec = (pbrt::Spectrum::FromRGB(sigARGB) * scale * density);
    const auto sigSSpec = (pbrt::Spectrum::FromRGB(sigSRGB) * scale * density);

    return new pbrt::HomogeneousMedium(sigASpec, sigSSpec, g);
}

pbrt::Medium* GridMediumFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto density = meta.getProperty<std::vector<float>>("density", {});
    const auto nx = meta.getProperty<int>("nx", 0);
    const auto ny = meta.getProperty<int>("ny", 0);
    const auto nz = meta.getProperty<int>("nz", 0);

    if(static_cast<int>(density.size()) != (nx*ny*nz))
    {
        BRAYNS_WARN << "PBRTModel: Attempted to add a grid volume whose dimensions did not "
                    << "match the number of density values given" << std::endl;
        return nullptr;
    }

    const auto g = static_cast<pbrt::Float>(meta.getProperty<double>("g", 0.0));
    const auto sigADArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {.0011f, .0024f, .014f});
    const auto sigSDArr = meta.getProperty<std::array<double, 3>>("sig_s",
                                                                  {2.55f, 3.21f, 3.77f});

    const auto scale = static_cast<pbrt::Float>(meta.getProperty<double>("scale", 1.0));
    const pbrt::Float sigARGB[3] = {static_cast<float>(sigADArr[0]),
                                    static_cast<float>(sigADArr[1]),
                                    static_cast<float>(sigADArr[2])};
    const pbrt::Float sigSRGB[3] = {static_cast<float>(sigSDArr[0]),
                                    static_cast<float>(sigSDArr[1]),
                                    static_cast<float>(sigSDArr[2])};


    const auto sigASpec = (pbrt::Spectrum::FromRGB(sigARGB) * scale);
    const auto sigSSpec = (pbrt::Spectrum::FromRGB(sigSRGB) * scale);

    std::vector<pbrt::Float> pbrtDensity (density.begin(), density.end());
    return new pbrt::GridDensityMedium(sigASpec, sigSSpec, g, nx, ny, nz, *otw, pbrtDensity.data());
}

template<class ShapeType>
std::shared_ptr<pbrt::AreaLight> createLightForShape(std::shared_ptr<ShapeType>& shape,
                                                     const brayns::TransferFunction& tf,
                                                     MaterialPtr& mat,
                                                     const float value)
{
    const auto cvalue = glm::clamp(static_cast<double>(value),
                                   tf.getValuesRange().x,
                                   tf.getValuesRange().y);
    float intensity = static_cast<float>((cvalue - tf.getValuesRange().x)
                                               / (tf.getValuesRange().y - tf.getValuesRange().x));

    const brayns::Vector3f color = tf.getColorForValue(value);
    if(mat)
    {
        if(intensity < 0.1f)
            mat->setDiffuseColor({1.f, 1.f, 1.f});
        else
            mat->setDiffuseColor({color.r, color.g, color.b});
    }

    // Do not add light if the intensity is below a given threshold
    if(intensity < 0.75f)
        return std::shared_ptr<pbrt::AreaLight>{nullptr};

    // remap intensity if we are adding light
    intensity = (intensity - 0.75f) / 0.25f;
    // smoothsetep light intensity
    const float lightIntensity = (intensity*intensity*intensity*
                                 (intensity*(intensity * 6 - 15) + 10)) * 3.f;

    pbrt::ParamSet params;

    pbrt::Float rgb[] = {pbrt::Float(std::fabs(color.r)),
                         pbrt::Float(std::fabs(color.g)),
                         pbrt::Float(std::fabs(color.b))};
    pbrt::Spectrum
            tempL = pbrt::Spectrum::FromRGB(rgb, pbrt::SpectrumType::Illuminant) * lightIntensity;
    std::unique_ptr<pbrt::Float[]> L (new pbrt::Float[3]);
    tempL.ToRGB(L.get());
    params.AddRGBSpectrum("L", std::move(L), 3);

    std::unique_ptr<int[]> nsamples (new int[1]);
    nsamples.get()[0] = 8;
    params.AddInt("samples", std::move(nsamples), 1);

    return pbrt::CreateDiffuseAreaLight(pbrt::Transform(), nullptr, params, shape);
}

} // namespace

std::unordered_map<std::string, PBRTModel::MediaFactory> PBRTModel::_mediaFactories = {
    {"homogeneus", HomogeneusMediumFactory},
    {"heterogeneus", HeterogeneusMediumFactory},
    {"grid", GridMediumFactory}
};

PBRTModel::PBRTModel(AnimationParameters& animationParameters,
                     VolumeParameters& volumeParameters)
    : Model(animationParameters, volumeParameters)
{
}

void PBRTModel::commitGeometry()
{
    updateBounds();
    // DO NOTHING, WE NEED THE TRANSFORMATION AT THE
    // CREATION TIME OF THE SHAPES.
}

std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
PBRTModel::commitToPBRT(const Transformation &transform, const std::string& renderer)
{
    _modelLights.clear();

    // Create materials before shapes, as they are required when building those
    _commitMaterials(renderer);

    // Both ways transforms
    _objectToWorld = pbrtTransform(transform);
    _worldToObject = pbrt::Transform (_objectToWorld.GetInverseMatrix(),
                                      _objectToWorld.GetMatrix());

    // Parse medium which will be added *inside* the model shapes
    _parseMedium(&_objectToWorld);

    // Create shapes
    auto spheres = _createSpheres(&_objectToWorld, &_worldToObject);
    auto cylinders = _createCylinders(&_objectToWorld, &_worldToObject);
    auto cones = _createCones(&_objectToWorld, &_worldToObject);
    auto meshes = _createMeshes(&_objectToWorld, &_worldToObject);
    auto sdfGeoms = _createSDFGeometries(&_objectToWorld, &_worldToObject);

    // Insert all into the result
    Primitives result;
    result.insert(result.end(), spheres.begin(), spheres.end());
    spheres.clear();
    result.insert(result.end(), cylinders.begin(), cylinders.end());
    cylinders.clear();
    result.insert(result.end(), cones.begin(), cones.end());
    cones.clear();
    result.insert(result.end(), meshes.begin(), meshes.end());
    meshes.clear();
    result.insert(result.end(), sdfGeoms.begin(), sdfGeoms.end());
    sdfGeoms.clear();

    updateBounds();
    _markGeometriesClean();
    _instancesDirty = false;

    return result;
}

void PBRTModel::buildBoundingBox()
{
    auto material = createMaterial(BOUNDINGBOX_MATERIAL_ID, "bounding_box");
    material->setDiffuseColor({1, 1, 1});
    material->setEmission(1.);
    material->commit();
}

SharedDataVolumePtr PBRTModel::createSharedDataVolume(const Vector3ui &dimensions,
                                                      const Vector3f &spacing,
                                                      const DataType type) const
{
    (void) dimensions;
    (void) spacing;
    (void) type;
    return SharedDataVolumePtr{nullptr};
}

MaterialPtr PBRTModel::createMaterialImpl(const PropertyMap& properties)
{
    return std::shared_ptr<Material>(new PBRTMaterial(properties));
}

bool PBRTModel::materialsDirty() const
{
    for(const auto& m : getMaterials())
        if(m.first != NO_MATERIAL && m.second->isModified())
            return true;

    return false;
}

void PBRTModel::_commitMaterials(const std::string& renderer)
{
    for(auto m : _materials)
    {
        if(m.first != NO_MATERIAL)
        {
            PBRTMaterial* pbrtM = static_cast<PBRTMaterial*>(m.second.get());
            if(!pbrtM)
                continue;

            pbrtM->commit(renderer);
        }
    }
}


std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
PBRTModel::_createSpheres(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    const pbrt::MediumInterface dummyMI (_modelMedium.get(), nullptr);

    for(const auto& sphereList : getSpheres())
    {
        MaterialPtr mat {nullptr};
        std::shared_ptr<pbrt::Material> pbrtMat = nullptr;
        if(sphereList.first != NO_MATERIAL && !_modelMedium)
        {
            mat = _materials[sphereList.first];
            PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());
            pbrtMat = impl.getPBRTMaterial();
        }

        for(const auto& sphere : sphereList.second)
        {
            pbrt::ParamSet params;

            const auto otwS = pbrt::Translate(pbrt::Vector3f(sphere.center.x,
                                                             sphere.center.y,
                                                             sphere.center.z));

            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              otwS.GetMatrix())));
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              otwS.GetInverseMatrix())));

            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

            auto radius = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
            radius.get()[0] = sphere.radius;
            params.AddFloat("radius", std::move(radius), 1);

            auto sphereShape = pbrt::CreateSphereShape(otwFinalPtr, wtoFinalPtr, false, params);

            // Generate a light if we have simulation
            std::shared_ptr<pbrt::AreaLight> dummyAL {nullptr};
            if(!_simulationData.empty())
            {
                dummyAL = createLightForShape(sphereShape, _transferFunction, mat,
                                              _simulationData[sphere.userData]);
                if(dummyAL)
                    _modelLights.push_back(dummyAL);
            }

            result.push_back(std::shared_ptr<pbrt::GeometricPrimitive>(
                                 new pbrt::GeometricPrimitive(
                                                        sphereShape,
                                                        pbrtMat,
                                                        dummyAL,
                                                        dummyMI)));
        }
    }

    return result;
}

std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
PBRTModel::_createCylinders(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    const pbrt::MediumInterface dummyMI (_modelMedium.get(), nullptr);

    for(const auto& cylinderList : getCylinders())
    {
        MaterialPtr mat {nullptr};
        std::shared_ptr<pbrt::Material> pbrtMaterial {nullptr};
        if(cylinderList.first != NO_MATERIAL && !_modelMedium)
        {
            mat = _materials[cylinderList.first];
            PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());
            pbrtMaterial = impl.getPBRTMaterial();
        }

        for(const auto& cylinder : cylinderList.second)
        {
            // Compute PBRT Transform from Brayns Transform
            const auto direction = glm::normalize(cylinder.up - cylinder.center);
            glm::vec3 theoreticalUp (0.f, 1.f, 0.f);
            if (direction == theoreticalUp)
                theoreticalUp = glm::vec3(1.f, 0.f, 0.f);
            auto rUp = glm::normalize(glm::cross(direction, theoreticalUp));
            auto pbrtMat = pbrt::LookAt(pbrt::Point3f(cylinder.center.x,
                                                      cylinder.center.y,
                                                      cylinder.center.z),
                                        pbrt::Point3f(cylinder.up.x,
                                                      cylinder.up.y,
                                                      cylinder.up.z),
                                        pbrt::Vector3f(rUp.x, rUp.y, rUp.z));

            const pbrt::Transform cylTrans(pbrtMat);

            // Multiply by model transformation
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              cylTrans.GetMatrix())));
            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              cylTrans.GetInverseMatrix())));

            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

            // Build cylinder paramset
            pbrt::ParamSet params;

            auto radius = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
            radius.get()[0] = cylinder.radius;
            params.AddFloat("radius", std::move(radius), 1);

            auto zmin = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
            zmin.get()[0] = 0.f;
            params.AddFloat("zmin", std::move(zmin), 1);

            auto zmax = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
            zmax.get()[0] = glm::length(cylinder.up - cylinder.center);
            params.AddFloat("zmax", std::move(zmax), 1);

            auto cylinderShape = pbrt::CreateCylinderShape(otwFinalPtr, wtoFinalPtr,
                                                           false, params);

            // Generate a light if we have simulation
            std::shared_ptr<pbrt::AreaLight> dummyAL {nullptr};
            if(!_simulationData.empty())
            {
                dummyAL = createLightForShape(cylinderShape, _transferFunction, mat,
                                              _simulationData[cylinder.userData]);
                if(dummyAL)
                    _modelLights.push_back(dummyAL);
            }

            result.push_back(std::shared_ptr<pbrt::GeometricPrimitive>(
                                 new pbrt::GeometricPrimitive(
                                                        cylinderShape,
                                                        pbrtMaterial,
                                                        dummyAL,
                                                        dummyMI)));
        }
    }

    return result;
}

std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
PBRTModel::_createCones(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    const pbrt::MediumInterface dummyMI (_modelMedium.get(), nullptr);

    for(const auto& coneList : getCones())
    {
        std::shared_ptr<Material> mat {nullptr};
        std::shared_ptr<pbrt::Material> pbrtMaterial {nullptr};
        if(coneList.first != NO_MATERIAL && !_modelMedium)
        {
            mat = _materials[coneList.first];
            PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());
            pbrtMaterial = impl.getPBRTMaterial();
        }


        for(const auto& cone : coneList.second)
        {
            // Compute PBRT Transform from Brayns Transform
            const auto direction = glm::normalize(cone.up - cone.center);
            glm::vec3 theoreticalUp (0.f, 1.f, 0.f);
            if (direction == theoreticalUp)
                theoreticalUp = glm::vec3(1.f, 0.f, 0.f);
            auto rUp = glm::normalize(glm::cross(direction, theoreticalUp));
            auto pbrtMat = pbrt::LookAt(pbrt::Point3f(cone.center.x, cone.center.y, cone.center.z),
                                        pbrt::Point3f(cone.up.x, cone.up.y, cone.up.z),
                                        pbrt::Vector3f(rUp.x, rUp.y, rUp.z));

            const pbrt::Transform conTrans(pbrtMat);

            // Multiply by model transformation
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              conTrans.GetMatrix())));
            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              conTrans.GetInverseMatrix())));

            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

            // Build cone paramset
            pbrt::ParamSet params;

            auto radius = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
            radius.get()[0] = cone.centerRadius;
            params.AddFloat("radius", std::move(radius), 1);

            auto height = std::unique_ptr<pbrt::Float[]>(new pbrt::Float[1]);
            height.get()[0] = glm::length(cone.up - cone.center);
            params.AddFloat("height", std::move(height), 1);

            auto coneShape = CreateCustomConeShape(otwFinalPtr, wtoFinalPtr, false, params);

            // Generate a light if we have simulation
            std::shared_ptr<pbrt::AreaLight> dummyAL {nullptr};
            if(!_simulationData.empty())
            {
                dummyAL = createLightForShape<CustomCone>(coneShape, _transferFunction,
                                                          mat, _simulationData[cone.userData]);
                if(dummyAL)
                    _modelLights.push_back(dummyAL);
            }

            result.push_back(std::shared_ptr<pbrt::GeometricPrimitive>(
                                 new pbrt::GeometricPrimitive(
                                                        coneShape,
                                                        pbrtMaterial,
                                                        dummyAL,
                                                        dummyMI)));
        }
    }

    return result;
}

std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
PBRTModel::_createMeshes(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    const pbrt::MediumInterface dummyMI (_modelMedium.get(), nullptr);

    const std::shared_ptr<pbrt::AreaLight> dummyAL;

    for(const auto& meshList : getTriangleMeshes())
    {
        std::shared_ptr<pbrt::Material> pbrtMaterial {nullptr};
        if(meshList.first != NO_MATERIAL && !_modelMedium)
        {
            const auto& mat = getMaterial(meshList.first);
            PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());
            pbrtMaterial = impl.getPBRTMaterial();
        }

        const auto& mesh = meshList.second;

        // Build mesh paramset
        pbrt::ParamSet params;

        // Triangles
        std::unique_ptr<int[]> indices (new int[mesh.indices.size() * 3]);
        for(size_t i = 0; i < mesh.indices.size(); ++i)
        {
            const size_t idx = i * 3;
            indices[idx] = static_cast<int>(mesh.indices[i].x);
            indices[idx+1] = static_cast<int>(mesh.indices[i].y);
            indices[idx+2] = static_cast<int>(mesh.indices[i].z);
        }
        params.AddInt("indices", std::move(indices), static_cast<int>(mesh.indices.size()) * 3);

        // Verties
        std::unique_ptr<pbrt::Point3f[]> vertices (new pbrt::Point3f[mesh.vertices.size()]);
        for(size_t i = 0; i < mesh.vertices.size(); ++i)
            vertices[i] = pbrt::Point3f(mesh.vertices[i].x,
                                        mesh.vertices[i].y,
                                        mesh.vertices[i].z);
        params.AddPoint3f("P", std::move(vertices), static_cast<int>(mesh.vertices.size()));

        // UVs
        if(!mesh.textureCoordinates.empty() &&
           mesh.textureCoordinates.size() == mesh.vertices.size())
        {
            std::unique_ptr<pbrt::Point2f[]>
                    uvs (new pbrt::Point2f[mesh.textureCoordinates.size()]);
            for(size_t i = 0; i < mesh.textureCoordinates.size(); ++i)
                uvs[i] = pbrt::Point2f (mesh.textureCoordinates[i].x,
                                        mesh.textureCoordinates[i].y);
            params.AddPoint2f("uv", std::move(uvs),
                              static_cast<int>(mesh.textureCoordinates.size()));
        }

        if(!mesh.normals.empty() && mesh.normals.size() == mesh.vertices.size())
        {
            std::unique_ptr<pbrt::Normal3f[]> normals (new pbrt::Normal3f[mesh.normals.size()]);
            for(size_t i = 0; i < mesh.normals.size(); ++i)
                normals[i] = pbrt::Normal3f(mesh.normals[i].x,
                                            mesh.normals[i].y,
                                            mesh.normals[i].z);
            params.AddNormal3f("N", std::move(normals), static_cast<int>(mesh.normals.size()));
        }

        auto pbrtMeshList = pbrt::CreateTriangleMeshShape(otw, wto, false, params);
        for(auto& meshObj : pbrtMeshList)
            result.push_back(std::shared_ptr<pbrt::GeometricPrimitive>(
                                 new pbrt::GeometricPrimitive(
                                                        meshObj,
                                                        pbrtMaterial,
                                                        dummyAL,
                                                        dummyMI)));
    }

    return result;
}

std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
PBRTModel::_createSDFGeometries(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    const pbrt::MediumInterface dummyMI (_modelMedium.get(), nullptr);
    const std::shared_ptr<pbrt::AreaLight> dummyAL;

    for(const auto& geometryList : getSDFGeometryData().geometryIndices)
    {
        std::shared_ptr<pbrt::Material> pbrtMaterial {nullptr};
        if(geometryList.first != NO_MATERIAL && !_modelMedium)
        {
            const auto& mat = getMaterial(geometryList.first);
            PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());
            pbrtMaterial = impl.getPBRTMaterial();
        }

        for(const auto gemetryIndex : geometryList.second)
        {
            const SDFGeometry& geom = getSDFGeometryData().geometries[gemetryIndex];

            std::shared_ptr<pbrt::Shape> shape;
            switch(geom.type)
            {
                case SDFType::Sphere:
                    shape = std::make_shared<PBRTSDFGeometryShape<SDFType::Sphere>>(otw, wto, false, &geom, &getSDFGeometryData());
                    break;
                case SDFType::Pill:
                    shape = std::make_shared<PBRTSDFGeometryShape<SDFType::Pill>>(otw, wto, false, &geom, &getSDFGeometryData());
                    break;
                case SDFType::ConePill:
                    shape = std::make_shared<PBRTSDFGeometryShape<SDFType::ConePill>>(otw, wto, false, &geom, &getSDFGeometryData());
                    break;
                case SDFType::ConePillSigmoid:
                    shape = std::make_shared<PBRTSDFGeometryShape<SDFType::ConePillSigmoid>>(otw, wto, false, &geom, &getSDFGeometryData());
                    break;

            }            

            result.push_back(std::make_shared<pbrt::GeometricPrimitive>(
                                                        shape,
                                                        pbrtMaterial,
                                                        dummyAL,
                                                        dummyMI));
        }
    }

    return result;
}

void PBRTModel::_parseMedium(pbrt::Transform *otw)
{
    // Release any current medium
    _modelMedium.reset();

    // We only allow one medium per model. Accessing individual shapes
    // from a model to add different mediums to each is not easy, and
    // we must rely on material Ids, which is not an intuitive or easy
    // way to identify them.
    // For multiple mediums, add multiple models
    bool foundValid = false;
    for(const auto& metaObjectList : getMetaObjects())
    {
        if(foundValid)
            break;

        for(const auto& metaObject : metaObjectList.second)
        {
            // Only interested in volume metaobjects
            if(!metaObject.hasProperty("volume_type"))
                continue;

            // Attempt to create a medium if the type is correct and all the
            // parameters fulfill the specifications
            const auto volType = metaObject.getProperty<std::string>("volume_type");
            auto factorIt = _mediaFactories.find(volType);
            if(factorIt == _mediaFactories.end())
                continue;

            auto mediaPtr = factorIt->second(otw, metaObject);
            if(mediaPtr != nullptr)
            {
                _modelMedium.reset(mediaPtr);
                foundValid = true;
            }
        }
    }
}

}
