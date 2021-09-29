#include "TestRenderer.h"

#include <ospray/SDK/common/Data.h>
#include <ospray/SDK/lights/Light.h>

#include "TestRenderer_ispc.h"

namespace circuitRenderer
{
void TestRenderer::commit()
{
    Renderer::commit();

    _lightData = (ospray::Data*)getParamData("lights");
    _lights.clear();

    if (_lightData)
    {
        for (size_t i = 0; i < _lightData->size(); ++i)
            _lights.push_back(((ospray::Light**)_lightData->data)[i]->getIE());

        _lightPtr = _lights.empty() ? nullptr : &_lights[0];
        _backgroundMaterial =
            (TestMaterial*)getParamObject("bgMaterial", nullptr);
    }

    ispc::TestRenderer_set(getIE(),
                           (_backgroundMaterial ? _backgroundMaterial->getIE()
                                                : nullptr),
                           spp, _lightPtr, _lights.size());
}

TestRenderer::TestRenderer()
{
    ispcEquivalent = ispc::TestRenderer_create(this);
}

// OSP_REGISTER_RENDERER(TestRenderer, TEST);
OSP_REGISTER_RENDERER(TestRenderer, test);
} // namespace circuitRenderer
