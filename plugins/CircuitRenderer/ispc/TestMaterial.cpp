#include "TestMaterial.h"
#include "TestMaterial_ispc.h"

#include <ospray/SDK/common/Data.h>

namespace circuitRenderer
{
void TestMaterial::commit()
{
    if (ispcEquivalent == nullptr)
        ispcEquivalent = ispc::TestMaterial_create(this);

    // NAMES ARE WRONG!!! THEY ARE SET AT OSPRAYMATERIAL.CPP IN ENGINES/OSPRAY
    opacity = getParam1f("d", 1.f);
    refraction = getParam1f("refraction", 1.f);
    reflection = getParam1f("reflection", 1.f);
    Kd = getParam3f("kd", ospray::vec3f(.8f));
    Ks = getParam3f("ks", ospray::vec3f(1.f));
    specularExponent = getParam1f("ns", 7.f);
    glossiness = getParam1f("glossiness", .5f);

    ispc::TestMaterial_set(getIE(), opacity, refraction, reflection,
                           (ispc::vec3f&)Kd, (ispc::vec3f&)Ks, specularExponent,
                           glossiness);
}

OSP_REGISTER_MATERIAL(test, TestMaterial, default);
} // namespace circuitRenderer
