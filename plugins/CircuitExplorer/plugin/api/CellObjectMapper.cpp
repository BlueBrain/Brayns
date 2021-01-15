#include "CellObjectMapper.h"

#include <brayns/common/types.h>

#include <common/commonTypes.h>
#include <common/log.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/engine/Material.h>

#define NB_MATERIALS_PER_INSTANCE 3

// ------------------------------------------------------------------------------

void CellObjectMapper::setSourceModel(brayns::ModelDescriptorPtr model)
{
    _model = model;
}

void CellObjectMapper::add(const size_t gid, const MorphologyMap& mm)
{
    _cellToRenderableMap[gid] = mm;
}

void CellObjectMapper::remove(const size_t gid)
{
    auto it = _cellToRenderableMap.find(gid);
    if(it != _cellToRenderableMap.end())
        _cellToRenderableMap.erase(it);
}

RemapCircuitResult
CellObjectMapper::remapCircuitColors(const CircuitColorScheme scheme,
                                     brayns::Scene& scene)
{
    RemapCircuitResult result;
    result.error = 0;
    result.message = "";

    if(scheme == _lastScheme)
    {
        result.error = 2;
        result.message = "The requested scheme matches the active one";
        return result;
    }

    PLUGIN_INFO << "Remapping circuit..." << std::endl;

    // Gather current geometry mapping (material -> geometry list)
    brayns::SpheresMap& srcSpheres = _model->getModel().getSpheres();
    brayns::CylindersMap& srcCylinder = _model->getModel().getCylinders();
    brayns::ConesMap& srcCones = _model->getModel().getCones();
    brayns::SDFBeziersMap& srcBeziers = _model->getModel().getSDFBeziers();
    brayns::TriangleMeshMap& srcMeshes = _model->getModel().getTriangleMeshes();

    // New geometry mapping storage
    brayns::SpheresMap remappedSpheres;
    brayns::CylindersMap remappedCylinder;
    brayns::ConesMap remappedCones;
    brayns::SDFBeziersMap remappedBeziers;
    brayns::TriangleMeshMap remappedMeshes;
    brayns::SDFGeometryData remappedSDFGeometries = _model->getModel().getSDFGeometryData();
    remappedSDFGeometries.geometryIndices.clear();

    // Create the new model which will replace the current one
    auto newModel = scene.createModel();

    // Clear current scheme to material mapping
    _data.etypes.materialMap.clear();
    _data.layers.materialMap.clear();
    _data.mtypes.materialMap.clear();
    _data.targets.materialMap.clear();

    try
    {
        // Copy the simulation handler
        auto sh = _model->getModel().getSimulationHandler();
        if(sh)
        {
            newModel->setSimulationHandler(sh);
            // Reset frame to 0, animation parameters will set the appropiate frame number later,
            // tiggering in the process the frame upload
            sh->setCurrentFrame(std::numeric_limits<uint32_t>::max());
        }

        // Copy the transfer function (only use for simulation)
        auto& tf = _model->getModel().getTransferFunction();
        newModel->getTransferFunction().setControlPoints(tf.getControlPoints());
        newModel->getTransferFunction().setColorMap(tf.getColorMap());
        newModel->getTransferFunction().setValuesRange(tf.getValuesRange());

        // Create the new materials given the new scheme
        brayns::PropertyMap materialProps;
        materialProps.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, sh? true : false});
        materialProps.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                                   static_cast<int>(MaterialShadingMode::diffuse)});
        materialProps.setProperty(
            {MATERIAL_PROPERTY_CLIPPING_MODE,
             static_cast<int>(MaterialClippingMode::no_clipping)});
        for(size_t i = 0; i < _cellToRenderableMap.size(); ++i)
        {
            const auto matId = _computeMaterialId(scheme, i);
            auto mptr = newModel->createMaterial(matId, std::to_string(matId), materialProps);
            if(sh)
                sh->bind(mptr);
        }

        // Remap item by item so we dont need to keep a whole second copy of
        // the circuit
        // Remap spheres
        for(auto& pair : _cellToRenderableMap)
        {
            // If the model has spheres
            auto& msm = pair.second._sphereMap;
            if(!msm.empty())
            {
                // Recompute the material id for this morphology
                const auto matId = _computeMaterialId(scheme, pair.second._linealIndex);
                // Add all the spheres of this morphology to the temporal storage
                auto& tempStorage = remappedSpheres[matId];
                // At the same time, rebuild the new morphology material id -> part map
                std::vector<size_t> newMapping;
                for(auto& spheres : msm)
                {
                    auto& src = srcSpheres[spheres.first];

                    for(auto sphereIndx : spheres.second)
                    {
                        newMapping.push_back(tempStorage.size());
                        tempStorage.push_back(src[sphereIndx]);
                    }
                }

                // Update morphology map
                pair.second._sphereMap.clear();
                pair.second._sphereMap[matId] = newMapping;
            }
        }
        newModel->getSpheres().insert(remappedSpheres.begin(), remappedSpheres.end());

        // Remap cylinders
        for(auto& pair : _cellToRenderableMap)
        {
            auto& msm = pair.second._cylinderMap;
            if(!msm.empty())
            {
                const auto matId = _computeMaterialId(scheme, pair.second._linealIndex);
                auto& tempStorage = remappedCylinder[matId];
                std::vector<size_t> newMapping;
                for(auto& cylinders : msm)
                {
                    auto& src = srcCylinder[cylinders.first];
                    for(auto cylinderIndx : cylinders.second)
                    {
                        newMapping.push_back(tempStorage.size());
                        tempStorage.push_back(src[cylinderIndx]);
                    }
                }

                pair.second._cylinderMap.clear();
                pair.second._cylinderMap[matId] = newMapping;
            }
        }
        newModel->getCylinders().insert(remappedCylinder.begin(), remappedCylinder.end());

        // Remap cones
        for(auto& pair : _cellToRenderableMap)
        {
            auto& msm = pair.second._coneMap;
            if(!msm.empty())
            {
                const auto matId = _computeMaterialId(scheme, pair.second._linealIndex);
                auto& tempStorage = remappedCones[matId];
                std::vector<size_t> newMapping;
                for(auto& cones : msm)
                {
                    auto& src = srcCones[cones.first];
                    for(auto coneIndx : cones.second)
                    {
                        newMapping.push_back(tempStorage.size());
                        tempStorage.push_back(src[coneIndx]);
                    }
                }

                pair.second._coneMap.clear();
                pair.second._coneMap[matId] = newMapping;
            }
        }
        newModel->getCones().insert(remappedCones.begin(), remappedCones.end());

        // Remap Bezier SDFs
        for(auto& pair : _cellToRenderableMap)
        {
            auto& msm = pair.second._sdfBezierMap;
            if(!msm.empty())
            {
                const auto matId = _computeMaterialId(scheme, pair.second._linealIndex);
                auto& tempStorage = remappedBeziers[matId];
                std::vector<size_t> newMapping;
                for(auto& beziers : msm)
                {
                    auto& src = srcBeziers[beziers.first];
                    for(auto bezierIndx : beziers.second)
                    {
                        newMapping.push_back(tempStorage.size());
                        tempStorage.push_back(src[bezierIndx]);
                    }
                }

                pair.second._sdfBezierMap.clear();
                pair.second._sdfBezierMap[matId] = newMapping;
            }
        }
        newModel->getSDFBeziers().insert(remappedBeziers.begin(), remappedBeziers.end());

        // Remap meshes
        for(auto& pair : _cellToRenderableMap)
        {
            if(pair.second._hasMesh)
            {
                const auto matId = _computeMaterialId(scheme, pair.second._linealIndex);
                remappedMeshes[matId] = srcMeshes[pair.second._triangleIndx];
                pair.second._triangleIndx = matId;
            }
        }
        newModel->getTriangleMeshes().insert(remappedMeshes.begin(), remappedMeshes.end());

        // Remap Geometry SDFs
        for(auto& pair : _cellToRenderableMap)
        {
            auto& msm = pair.second._sdfGeometryMap;
            if(!msm.empty())
            {
                const auto matId = _computeMaterialId(scheme, pair.second._linealIndex);
                auto& tempStorage = remappedSDFGeometries.geometryIndices[matId];
                for(auto& geometries : msm)
                {
                    for(auto geometryIndx : geometries.second)
                        tempStorage.push_back(geometryIndx);
                }
            }
        }
        newModel->getSDFGeometryData() = remappedSDFGeometries;

        // Model descriptor to hold the model
        brayns::ModelDescriptorPtr modelDescriptor;

        // Copy previous model transformation
        brayns::Transformation transformation = _model->getTransformation();

        // Create model descriptor
        modelDescriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(newModel), _model->getName(),
                                                      _model->getPath(),
                                                      _model->getMetadata());
        modelDescriptor->setTransformation(transformation);

        const size_t oldId = _model->getModelID();
        _model = modelDescriptor;

        // After remapping is done, execute needed tasks
        // (Modify metadata, etc)
        onCircuitColorFinish(scheme, MorphologyColorScheme::none);

        // Set the new material colors to Brayn's default circuit color map
        _applyDefaultColorMap();

        // Replace current model with the new one
        scene.addModel(oldId, modelDescriptor);

        _lastScheme = scheme;
        _lastMorphologyScheme = MorphologyColorScheme::none;
    }
    catch(std::exception& e)
    {
        result.error = 3;
        result.message = "An exception occoured: " + std::string(e.what());
    }

    PLUGIN_INFO << "Remapping done!" << std::endl;

    return result;
}

void CellObjectMapper::remapMorphologyColors(const MorphologyColorScheme scheme)
{
    if(scheme == _lastMorphologyScheme)
        return;

    _lastMorphologyScheme = scheme;
    _lastScheme = CircuitColorScheme::none;
}

void CellObjectMapper::onCircuitColorFinish(const CircuitColorScheme& scheme,
                                            const MorphologyColorScheme& mScheme)
{
    if(scheme != CircuitColorScheme::none)
    {
        if(scheme == CircuitColorScheme::by_layer)
        {
            std::string materialGroupsData = "[";
            const LayerSchemeItem& si = _data.layers;
            std::set<std::string> uniqueIds;
            uniqueIds.insert(si.ids.begin(), si.ids.end());
            size_t counter = 0;
            for(const auto& id : uniqueIds)
            {
                auto matIdIt = si.materialMap.find(id);
                if(matIdIt != si.materialMap.end())
                {
                    counter++;
                    const std::string ending = counter == si.materialMap.size()? "" : ",";
                    materialGroupsData += "{\"name\":\""+id+"\", "
                                          "\"ids\":["+std::to_string(matIdIt->second)+"]}" + ending;
                }
            }
            materialGroupsData += "]";

            brayns::ModelMetadata newMetadata = _model->getMetadata();
            newMetadata["materialGroups"] = materialGroupsData;
            _model->setMetadata(newMetadata);
        }
        else
        {
            // If the "materialsGroup" field is present, remove it
            // when remapping to a different scheme than by layer
            const brayns::ModelMetadata& oldMetadata = _model->getMetadata();
            auto it = oldMetadata.find("");
            if(it != oldMetadata.end())
            {
                auto newMetadata = oldMetadata;
                newMetadata.erase("materialsGroup");
                _model->setMetadata(newMetadata);
            }
        }
    }
    else if(mScheme != MorphologyColorScheme::none)
    {

    }
}

CircuitSchemeData& CellObjectMapper::getSchemeData()
{
    return _data;
}

size_t CellObjectMapper::_computeMaterialId(const CircuitColorScheme scheme,
                                            const size_t index)
{
    size_t materialId = 0;
    switch (scheme)
    {
    case CircuitColorScheme::single_color:
        materialId = 1;
        break;
    case CircuitColorScheme::by_id:
        materialId = NB_MATERIALS_PER_INSTANCE * index;
        break;
    case CircuitColorScheme::by_target:
        if(!_data.targets.ids.empty())
        {
            for (size_t i = 0; i < _data.targets.ids.size() - 1; ++i)
                if (index >= _data.targets.ids[i] && index < _data.targets.ids[i + 1])
                {
                    materialId = NB_MATERIALS_PER_INSTANCE * i;
                    _data.targets.materialMap[i] = materialId;
                    break;
                }
        }
        break;
    case CircuitColorScheme::by_etype:
        if (index < _data.etypes.ids.size())
        {
            const auto etypeId = _data.etypes.ids[index];
            materialId =
                NB_MATERIALS_PER_INSTANCE * etypeId;
            _data.etypes.materialMap[etypeId] = materialId;
        }
        else
            PLUGIN_WARN << "Failed to get neuron E-type" << std::endl;
        break;
    case CircuitColorScheme::by_mtype:
        if (index < _data.mtypes.ids.size())
        {
            const auto mtypeId = _data.mtypes.ids[index];
            materialId = NB_MATERIALS_PER_INSTANCE * mtypeId;
            _data.mtypes.materialMap[mtypeId] = materialId;
        }
        else
            PLUGIN_WARN << "Failed to get neuron M-type" << std::endl;
        break;
    case CircuitColorScheme::by_layer:
        if (index < _data.layers.ids.size())
        {
            const auto& layerId = _data.layers.ids[index];
            auto it = _data.layers.virtualIndex.find(layerId);
            if(it != _data.layers.virtualIndex.end())
            {
                materialId = NB_MATERIALS_PER_INSTANCE * it->second;
                _data.layers.materialMap[layerId] = materialId;
            }
            else
                PLUGIN_WARN << "Failed to get layer virtual index" << std::endl;
        }
        else
            PLUGIN_WARN << "Failed to get neuron layer" << std::endl;
        break;
    default:
        materialId = brayns::NO_MATERIAL;
    }

    return materialId;
}

void CellObjectMapper::_applyDefaultColorMap() const
{
    const static std::vector<brayns::Vector3d> colors = {
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804},
        {0.30196078431372547, 0.6862745098039216, 0.2901960784313726},
        {0.596078431372549, 0.3058823529411765, 0.6392156862745098},
        {1.0, 0.4980392156862745, 0.0},
        {1.0, 1.0, 0.2},
        {0.6509803921568628, 0.33725490196078434, 0.1568627450980392},
        {0.9686274509803922, 0.5058823529411764, 0.7490196078431373},
        {0.6, 0.6, 0.6},
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804},
        {0.30196078431372547, 0.6862745098039216, 0.2901960784313726},
        {0.596078431372549, 0.3058823529411765, 0.6392156862745098},
        {1.0, 0.4980392156862745, 0.0},
        {1.0, 1.0, 0.2},
        {0.6509803921568628, 0.33725490196078434, 0.1568627450980392},
        {0.9686274509803922, 0.5058823529411764, 0.7490196078431373},
        {0.6, 0.6, 0.6},
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804}};

    uint64_t i = 0;
    auto &materials = _model->getModel().getMaterials();
    for (auto &material : materials)
    {
        if(material.first != brayns::SECONDARY_MODEL_MATERIAL_ID
           && material.first != brayns::BOUNDINGBOX_MATERIAL_ID)
        {
            const auto &color = colors[i % colors.size()];
            material.second->setDiffuseColor(color);
            ++i;
        }
    }
}
