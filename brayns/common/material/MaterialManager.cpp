#include "MaterialManager.h"

#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>

namespace brayns
{
MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::clear()
{
    _materials.clear();
}

void MaterialManager::buildMissingMaterials(const size_t materialId)
{
    if (materialId >= _materials.size())
        _materials.resize(materialId + 1);
}

Material& MaterialManager::get(const size_t index)
{
    buildMissingMaterials(index);
    return _materials[index];
}

size_t MaterialManager::add(const Material& material)
{
    _materials.push_back(material);
    return _materials.size() - 1;
}

void MaterialManager::set(const size_t index, const Material& material)
{
    buildMissingMaterials(index);
    _materials[index] = material;
}

void MaterialManager::reset()
{
    BRAYNS_INFO << "Building system materials" << std::endl;
    clear();
    for (size_t i = 0; i < NB_SYSTEM_MATERIALS; ++i)
    {
        Material material;
        switch (MaterialType(i))
        {
        case MaterialType::bounding_box:
            material.setColor(Vector3f(1.f, 1.f, 1.f));
            material.setEmission(10.f);
            break;
        case MaterialType::invisible:
            material.setOpacity(0.f);
            material.setRefractionIndex(1.f);
            material.setColor(Vector3f(1.f, 1.f, 1.f));
            material.setSpecularColor(Vector3f(0.f, 0.f, 0.f));
            break;
        default:
            break;
        }
        _materials.push_back(material);
    }
}
}
