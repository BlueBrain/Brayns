#include "SomaCircuitComponent.h"

SomaCircuitComponent::SomaCircuitComponent(std::vector<brayns::Sphere> spheres)
 : brayns::GeometryRendererComponent<brayns::Sphere>()
{
    auto &geometry = getGeometry();
    geometry.set(std::move(spheres));
}

void SomaCircuitComponent::commitColor()
{
    // NOOP
}