#include <ospray/SDK/camera/PerspectiveCamera.ih>
#include <ospray/SDK/common/Model.ih>
#include <ospray/SDK/fb/FrameBuffer.ih>
#include <ospray/SDK/lights/Light.ih>
#include <ospray/SDK/render/Renderer.ih>

#include "TestMaterial.ih"

struct TestRenderer
{
    Renderer super;

    const uniform Light* uniform* uniform lights;
    uint32 numLights;
    TestMaterial* bgMaterial;
};


inline void composite(const varying vec4f& src, varying vec4f& dst,
                      const varying float alphaCorrection)
{
    const float alpha =
        1.f - pow(1.f - min(src.w, 1.f - 1.f / 256.f), alphaCorrection);
    const float a = alpha * (1.f - dst.w);
    dst.x = dst.x + src.x * a;
    dst.y = dst.y + src.y * a;
    dst.z = dst.z + src.z * a;
    dst.w += (alpha * (1.f - dst.w));
}

inline vec3f refractedVector(const varying vec3f& direction,
                             const varying vec3f& normal,
                             const varying float n1, const varying float n2)
{
    if (n2 == 0.f)
        return direction;
    const float eta = n1 / n2;
    const float cos1 = -dot(direction, normal);
    const float cos2 = 1.f - eta * eta * (1.f - cos1 * cos1);
    if (cos2 > 0.f)
        return eta * direction + (eta * cos1 - sqrt(cos2)) * normal;
    return direction;
}

inline vec3f TestRenderer_shadeRay(const uniform TestRenderer* uniform self,
                                   varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    varying vec3f color = make_vec3f(0.f);

    sample.z = inf;
    sample.alpha = 0.f;

    traceRay(self->super.model, ray);

    if (ray.geomID < 0)
    {
        // No intersection, background color
        sample.alpha = 1.0;
        return make_vec3f(self->bgMaterial->Kd);
    }

    DifferentialGeometry dg;
    postIntersect(self->super.model, dg, ray,
                  DG_NS | DG_NG | DG_NORMALIZE | DG_FACEFORWARD |
                      DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

    uniform TestMaterial* material = (uniform TestMaterial*)dg.material;

    vec3f Kd = make_vec3f(0.f);
    if (!material)
        Kd = make_vec3f(dg.color);
    else
        foreach_unique(mat in material)
        {
            Kd = mat->Kd * make_vec3f(dg.color);
        }

    // Head-light shading
    color = Kd * max(0.f, dot(neg(ray.dir), dg.Ns));
    sample.alpha = 1.f;

    // Z-Depth
    sample.z = ray.t;
    sample.alpha = 1.f;

    return color;
}

void TestRenderer_renderSample(uniform Renderer* uniform _self,
                                void* uniform perFrameData,
                                varying ScreenSample& sample)
{
    uniform TestRenderer* uniform self =
        (uniform TestRenderer * uniform)_self;
    //sample.ray.time = self->abstract.timestamp;
    sample.rgb = TestRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform TestRenderer_create(void* uniform cppE)
{
    uniform TestRenderer* uniform self = uniform new uniform TestRenderer;
    Renderer_Constructor(&self->super, cppE);
    self->super.renderSample = TestRenderer_renderSample;
    return self;
}

export void TestRenderer_set(void* uniform _self, void* uniform bgMaterial,
                              const uniform int& spp, void** uniform lights,
                              uniform int32 numLights)
{
    uniform TestRenderer* uniform self =
        (uniform TestRenderer * uniform)_self;

    self->bgMaterial = (uniform TestMaterial * uniform)bgMaterial;
    //self->timestamp = timestamp;
    self->super.spp = spp;

    self->lights = (const uniform Light* uniform* uniform)lights;
    self->numLights = numLights;
}
