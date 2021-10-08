#include "PBRTCustomCone.h"

#include <pbrt/core/efloat.h>
#include <pbrt/core/paramset.h>
#include <pbrt/core/sampling.h>

namespace brayns
{
// Cone Method Definitions
CustomCone::CustomCone(const pbrt::Transform* o2w, const pbrt::Transform* w2o,
                       bool ro, pbrt::Float height, pbrt::Float radius,
                       pbrt::Float phiMax)
    : Shape(o2w, w2o, ro)
    , radius(radius)
    , height(height)
    , phiMax(pbrt::Radians(pbrt::Clamp(phiMax, 0, 360)))
{
}

pbrt::Bounds3f CustomCone::ObjectBound() const
{
    pbrt::Point3f p1 = pbrt::Point3f(-radius, -radius, 0);
    pbrt::Point3f p2 = pbrt::Point3f(radius, radius, height);
    return pbrt::Bounds3f(p1, p2);
}

bool CustomCone::Intersect(const pbrt::Ray& r, pbrt::Float* tHit,
                           pbrt::SurfaceInteraction* isect, bool) const
{
    // ProfilePhase p(Prof::ShapeIntersect);
    pbrt::Float phi;
    pbrt::Point3f pHit;
    // Transform _Ray_ to object space
    pbrt::Vector3f oErr, dErr;
    pbrt::Ray ray = (*WorldToObject)(r, &oErr, &dErr);

    // Compute quadratic cone coefficients

    // Initialize _EFloat_ ray coordinate values
    pbrt::EFloat ox(ray.o.x, oErr.x), oy(ray.o.y, oErr.y), oz(ray.o.z, oErr.z);
    pbrt::EFloat dx(ray.d.x, dErr.x), dy(ray.d.y, dErr.y), dz(ray.d.z, dErr.z);
    pbrt::EFloat k = pbrt::EFloat(radius) / pbrt::EFloat(height);
    k = k * k;
    pbrt::EFloat a = dx * dx + dy * dy - k * dz * dz;
    pbrt::EFloat b = 2 * (dx * ox + dy * oy - k * dz * (oz - height));
    pbrt::EFloat c = ox * ox + oy * oy - k * (oz - height) * (oz - height);

    // Solve quadratic equation for _t_ values
    pbrt::EFloat t0, t1;
    if (!Quadratic(a, b, c, &t0, &t1))
        return false;

    // Check quadric shape _t0_ and _t1_ for nearest intersection
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    pbrt::EFloat tShapeHit = t0;
    if (tShapeHit.LowerBound() <= 0)
    {
        tShapeHit = t1;
        if (tShapeHit.UpperBound() > ray.tMax)
            return false;
    }

    // Compute cone inverse mapping
    pHit = ray((pbrt::Float)tShapeHit);
    phi = std::atan2(pHit.y, pHit.x);
    if (phi < 0.)
        phi += 2 * pbrt::Pi;

    // Test cone intersection against clipping parameters
    if (pHit.z < 0 || pHit.z > height || phi > phiMax)
    {
        if (tShapeHit == t1)
            return false;
        tShapeHit = t1;
        if (t1.UpperBound() > ray.tMax)
            return false;
        // Compute cone inverse mapping
        pHit = ray(static_cast<pbrt::Float>(tShapeHit));
        phi = std::atan2(pHit.y, pHit.x);
        if (phi < 0.)
            phi += 2 * pbrt::Pi;
        if (pHit.z < 0 || pHit.z > height || phi > phiMax)
            return false;
    }

    // Find parametric representation of cone hit
    pbrt::Float u = phi / phiMax;
    pbrt::Float v = pHit.z / height;

    // Compute cone $\dpdu$ and $\dpdv$
    pbrt::Vector3f dpdu(-phiMax * pHit.y, phiMax * pHit.x, 0);
    pbrt::Vector3f dpdv(-pHit.x / (1.f - v), -pHit.y / (1.f - v), height);

    // Compute cone $\dndu$ and $\dndv$
    pbrt::Vector3f d2Pduu =
        -phiMax * phiMax * pbrt::Vector3f(pHit.x, pHit.y, 0.);
    pbrt::Vector3f d2Pduv =
        phiMax / (1.f - v) * pbrt::Vector3f(pHit.y, -pHit.x, 0.);
    pbrt::Vector3f d2Pdvv(0, 0, 0);

    // Compute coefficients for fundamental forms
    pbrt::Float E = Dot(dpdu, dpdu);
    pbrt::Float F = Dot(dpdu, dpdv);
    pbrt::Float G = Dot(dpdv, dpdv);
    pbrt::Vector3f N = pbrt::Normalize(pbrt::Cross(dpdu, dpdv));
    pbrt::Float e = Dot(N, d2Pduu);
    pbrt::Float f = Dot(N, d2Pduv);
    pbrt::Float g = Dot(N, d2Pdvv);

    // Compute $\dndu$ and $\dndv$ from fundamental form coefficients
    pbrt::Float invEGF2 = 1 / (E * G - F * F);
    pbrt::Normal3f dndu = pbrt::Normal3f((f * F - e * G) * invEGF2 * dpdu +
                                         (e * F - f * E) * invEGF2 * dpdv);
    pbrt::Normal3f dndv = pbrt::Normal3f((g * F - f * G) * invEGF2 * dpdu +
                                         (f * F - g * E) * invEGF2 * dpdv);

    // Compute error bounds for cone intersection

    // Compute error bounds for intersection computed with ray equation
    pbrt::EFloat px = ox + tShapeHit * dx;
    pbrt::EFloat py = oy + tShapeHit * dy;
    pbrt::EFloat pz = oz + tShapeHit * dz;
    pbrt::Vector3f pError =
        pbrt::Vector3f(px.GetAbsoluteError(), py.GetAbsoluteError(),
                       pz.GetAbsoluteError());

    // Initialize _SurfaceInteraction_ from parametric information
    *isect = (*ObjectToWorld)(
        pbrt::SurfaceInteraction(pHit, pError, pbrt::Point2f(u, v), -ray.d,
                                 dpdu, dpdv, dndu, dndv, ray.time, this));
    *tHit = static_cast<pbrt::Float>(tShapeHit);
    return true;
}

bool CustomCone::IntersectP(const pbrt::Ray& r, bool) const
{
    // ProfilePhase p(Prof::ShapeIntersectP);
    pbrt::Float phi;
    pbrt::Point3f pHit;
    // Transform _Ray_ to object space
    pbrt::Vector3f oErr, dErr;
    pbrt::Ray ray = (*WorldToObject)(r, &oErr, &dErr);

    // Compute quadratic cone coefficients

    // Initialize _EFloat_ ray coordinate values
    pbrt::EFloat ox(ray.o.x, oErr.x), oy(ray.o.y, oErr.y), oz(ray.o.z, oErr.z);
    pbrt::EFloat dx(ray.d.x, dErr.x), dy(ray.d.y, dErr.y), dz(ray.d.z, dErr.z);
    pbrt::EFloat k = pbrt::EFloat(radius) / pbrt::EFloat(height);
    k = k * k;
    pbrt::EFloat a = dx * dx + dy * dy - k * dz * dz;
    pbrt::EFloat b = 2 * (dx * ox + dy * oy - k * dz * (oz - height));
    pbrt::EFloat c = ox * ox + oy * oy - k * (oz - height) * (oz - height);

    // Solve quadratic equation for _t_ values
    pbrt::EFloat t0, t1;
    if (!Quadratic(a, b, c, &t0, &t1))
        return false;

    // Check quadric shape _t0_ and _t1_ for nearest intersection
    if (t0.UpperBound() > ray.tMax || t1.LowerBound() <= 0)
        return false;
    pbrt::EFloat tShapeHit = t0;
    if (tShapeHit.LowerBound() <= 0)
    {
        tShapeHit = t1;
        if (tShapeHit.UpperBound() > ray.tMax)
            return false;
    }

    // Compute cone inverse mapping
    pHit = ray((pbrt::Float)tShapeHit);
    phi = std::atan2(pHit.y, pHit.x);
    if (phi < 0.)
        phi += 2 * pbrt::Pi;

    // Test cone intersection against clipping parameters
    if (pHit.z < 0 || pHit.z > height || phi > phiMax)
    {
        if (tShapeHit == t1)
            return false;
        tShapeHit = t1;
        if (t1.UpperBound() > ray.tMax)
            return false;
        // Compute cone inverse mapping
        pHit = ray((pbrt::Float)tShapeHit);
        phi = std::atan2(pHit.y, pHit.x);
        if (phi < 0.)
            phi += 2 * pbrt::Pi;
        if (pHit.z < 0 || pHit.z > height || phi > phiMax)
            return false;
    }
    return true;
}

pbrt::Float CustomCone::Area() const
{
    return radius * std::sqrt((height * height) + (radius * radius)) * phiMax /
           2;
}

/**
 * CUSTOM IMPLEMENTATION. IS NOT SUPOSSED TO BE PHYSICALLY CORRECT
 */
pbrt::Interaction CustomCone::Sample(const pbrt::Point2f& u,
                                     pbrt::Float* pdf) const
{
    pbrt::Point3f pObj = pbrt::Point3f(0, 0, 0) +
                         radius * pbrt::UniformSampleCone(u, pbrt::Float(1));
    pbrt::Interaction it;
    it.n = Normalize((*ObjectToWorld)(pbrt::Normal3f(pObj.x, pObj.y, pObj.z)));
    if (reverseOrientation)
        it.n *= -1;
    // Reproject _pObj_ to sphere surface and compute _pObjError_
    pObj *= radius / Distance(pObj, pbrt::Point3f(0, 0, 0));
    pbrt::Vector3f pObjError = gamma(5) * Abs((pbrt::Vector3f)pObj);
    it.p = (*ObjectToWorld)(pObj, pObjError, &it.pError);
    *pdf = 1 / Area();
    return it;
}

std::shared_ptr<CustomCone> CreateCustomConeShape(const pbrt::Transform* o2w,
                                                  const pbrt::Transform* w2o,
                                                  bool reverseOrientation,
                                                  const pbrt::ParamSet& params)
{
    pbrt::Float radius = params.FindOneFloat("radius", 1);
    pbrt::Float height = params.FindOneFloat("height", 1);
    pbrt::Float phimax = params.FindOneFloat("phimax", 360);
    return std::make_shared<CustomCone>(o2w, w2o, reverseOrientation, height,
                                        radius, phimax);
}
} // namespace brayns
