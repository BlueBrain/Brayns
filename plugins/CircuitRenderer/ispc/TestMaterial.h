#ifndef TESTMATERIAL_H
#define TESTMATERIAL_H

#include <ospray/SDK/common/Material.h>

namespace circuitRenderer
{
struct TestMaterial : public ospray::Material
{
    float opacity;
    float refraction;
    float reflection;
    ospray::vec3f Kd;
    ospray::vec3f Ks;
    float specularExponent;
    float glossiness;

    std::string toString() const override
    {
        return "circuitRenderer::TestMaterial";
    }

    void commit() override;
};
}

#endif
