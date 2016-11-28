/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "OptiXUtils.h"
#include "OptiXCamera.h"

#include <brayns/common/log.h>
#include <plugins/engines/optix/OptiXUtils.h>
#include <optixu/optixpp_namespace.h>
#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_Camera.cu.ptx.h>
#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_Constantbg.cu.ptx.h>

namespace
{
char const* const CUDA_PERSPECTIVE_CAMERA = braynsOptiXCudaPlugin_generated_Camera_cu_ptx;
char const* const CUDA_MISS = braynsOptiXCudaPlugin_generated_Constantbg_cu_ptx;
const std::string CUDA_FUNCTION_CAMERA = "camera";
const std::string CUDA_FUNCTION_EXCEPTION = "exception";
const std::string CUDA_FUNCTION_ENVMAP_MISS = "envmap_miss";
const std::string CUDA_FUNCTION_MISS = "miss";

const std::string CUDA_ATTRIBUTE_BAD_COLOR = "bad_color";
const std::string CUDA_ATTRIBUTE_CAMERA_EYE = "eye";
const std::string CUDA_ATTRIBUTE_CAMERA_U = "U";
const std::string CUDA_ATTRIBUTE_CAMERA_V = "V";
const std::string CUDA_ATTRIBUTE_CAMERA_W = "W";
const std::string CUDA_ATTRIBUTE_CAMERA_APERTURE_RADIUS = "aperture_radius";
const std::string CUDA_ATTRIBUTE_CAMERA_FOCAL_SCALE = "focal_scale";
}

namespace brayns
{

OptiXCamera::OptiXCamera(
    const CameraType cameraType,
    optix::Context& context,
    const bool environmentMap )
    : Camera( cameraType )
    , _camera( 0 )
    , _exceptionProgram( 0 )
    , _context( context )
{
    std::string cameraName;
    std::string cameraPtx;
    switch (cameraType)
    {
    case CameraType::perspective:
        cameraName = CUDA_FUNCTION_CAMERA;
        cameraPtx = CUDA_PERSPECTIVE_CAMERA;
        break;
    default:
        BRAYNS_THROW( std::runtime_error("Unsupported camera") );
        break;
    }

    // Exception program
    _exceptionProgram = _context->createProgramFromPTXString( cameraPtx, CUDA_FUNCTION_EXCEPTION );
    _context->setExceptionProgram( 0, _exceptionProgram );
    _context[ CUDA_ATTRIBUTE_BAD_COLOR ]->setFloat( 1.f, 0.f, 1.f );

    // Miss program
    _context->setMissProgram( 0, _context->createProgramFromPTXString(
        CUDA_MISS, environmentMap ? CUDA_FUNCTION_ENVMAP_MISS : CUDA_FUNCTION_MISS ));

    // Ray generation program
    _camera = _context->createProgramFromPTXString( cameraPtx, cameraName );
}

void OptiXCamera::commit()
{
    Vector3f u, v, w;

    const Vector3f& pos = getPosition();

     _calculateCameraVariables( u, v, w );

    _context[ CUDA_ATTRIBUTE_CAMERA_EYE ]->setFloat( pos.x(), pos.y(), pos.z() );
    _context[ CUDA_ATTRIBUTE_CAMERA_U ]->setFloat( u.x(), u.y(), u.z() );
    _context[ CUDA_ATTRIBUTE_CAMERA_V ]->setFloat( v.x(), v.y(), v.z() );
    _context[ CUDA_ATTRIBUTE_CAMERA_W ]->setFloat( w.x(), w.y(), w.z() );
    _context[ CUDA_ATTRIBUTE_CAMERA_APERTURE_RADIUS ]->setFloat( getAperture( ));
    _context[ CUDA_ATTRIBUTE_CAMERA_FOCAL_SCALE ]->setFloat( getFocalLength( ));
}

void OptiXCamera::_calculateCameraVariables( Vector3f& U, Vector3f& V, Vector3f& W )
{
    float ulen, vlen, wlen;
    W = getTarget() - getPosition();

    wlen = W.length();
    U = normalize( cross( W, getUp( )));
    V = normalize( cross( U, W  ) );

    vlen = wlen * tanf( 0.5f * getFieldOfView() * M_PI / 180.f );
    V *= vlen;
    ulen = vlen * getAspectRatio();
    U *= ulen;
}

}
