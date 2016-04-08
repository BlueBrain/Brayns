/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "StereoCamera.ih"

void StereoCamera_initRay(uniform Camera *uniform _self,
                          varying Ray &ray,
                          const varying CameraSample &sample)
{
    uniform StereoCamera *uniform self =
            (uniform StereoCamera *uniform)_self;
    vec3f org = self->org;
    vec3f dir;
    if( sample.screen.x<0.5f )
    {
        dir = self->dir_00
                + (sample.screen.x-self->stereo) * (self->dir_du*2.f)
                + (sample.screen.y-0.25f) * self->dir_dv*2.f;
        org.x -= self->separation;
    }
    else
    {
        dir = self->dir_00
                + (sample.screen.x-0.5f+self->stereo) * (self->dir_du*2.f)
                + (sample.screen.y-0.25f) * self->dir_dv*2.f;
        org.x += self->separation;
    }
    setRay(ray,org,normalize(dir),self->nearClip,inf);
}

export void *uniform StereoCamera_create(void *uniform cppE)
{
    uniform StereoCamera *uniform cam
            = uniform new uniform StereoCamera;
    cam->super.cppEquivalent = cppE;
    cam->super.initRay = StereoCamera_initRay;
    cam->super.doesDOF = false;
    return cam;
}

export void StereoCamera_set(void *uniform _self,
                             const uniform vec3f &org,
                             const uniform vec3f &dir_00,
                             const uniform vec3f &dir_du,
                             const uniform vec3f &dir_dv,
                             const uniform float nearClip,
                             const uniform float stereo,
                             const uniform float separation)
{
    uniform StereoCamera *uniform self
            = (uniform StereoCamera *uniform)_self;
    self->org    = org;
    self->dir_00 = dir_00;
    self->dir_du = dir_du;
    self->dir_dv = dir_dv;
    self->nearClip = nearClip;
    self->stereo = stereo;
    self->separation = separation;
}
