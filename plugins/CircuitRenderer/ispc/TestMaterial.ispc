#include "TestMaterial.ih"

export void* uniform TestMaterial_create(void* uniform cppStub)
{
    uniform TestMaterial* uniform mat = uniform new uniform TestMaterial;
    mat->super.cppEquivalent = cppStub;
    return mat;
}

export void TestMaterial_set(
        void* uniform _mat,
        const uniform float& opacity,
        const uniform float& refraction,
        const uniform float& reflection,
        const uniform vec3f& Kd,
        const uniform vec3f& Ks,
        const uniform float& specularExponent,
        const uniform float& glossiness)
{
    uniform TestMaterial* uniform self =
       (uniform TestMaterial* uniform)_mat;

    self->opacity = opacity;
    self->refraction = refraction;
    self->reflection = reflection;
    self->Kd = Kd;
    self->Ks = Ks;
    self->specularExponent = specularExponent;
    self->glossiness = glossiness;
}
