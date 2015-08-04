/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef APPLICATIONPARAMETERS_H
#define APPLICATIONPARAMETERS_H

#include "AbstractParameters.h"

#include <string>

namespace brayns
{

/** Command line parameter data types
 */
enum SceneEnvironment
{
    seNone,
    seGround,
    seBox
};

/** Manages application parameters
 */
class ApplicationParameters : public AbstractParameters
{
public:
    ApplicationParameters();

    void parse(int argc, const char **argv) final;
    void display() const final;

    /** OSPRay module */
    std::string getModule() const { return module_; }

    /** OSPRay renderer */
    std::string getRenderer() const { return renderer_; }

    /** folder containing SWC files */
    std::string getSWCFolder() const { return swcFolder_; }

    /** folder containing PDB files */
    std::string getPDBFolder() const { return pdbFolder_; }

    /** folder containing H5 files */
    std::string getH5Folder() const { return h5Folder_; }

    /** folder containing mesh files */
    std::string getMeshFolder() const { return meshFolder_; }

    /** window width */
    size_t getWindowWidth() const { return windowWidth_; }

    /** window height */
    size_t getWindowHeight() const { return windowHeight_; }

    /** camera name (e.g. Perspective, Stereo, etc. ) */
    std::string getCamera() const { return camera_; }

    /** Scene environment (0: none, 1: ground, 2: box ) */
    SceneEnvironment getSceneEnvironment() const { return sceneEnvironment_; }

#ifdef BRAYNS_USE_RESTBRIDGE
    /** REST interface parameters */
    /** http host name */
    std::string getRESTHostname() const { return restHostname_; }
    /** http port */
    size_t getRESTPort() const { return restPort_; }
    /** ZeroEQ schema name */
    std::string getZeqSchema() const { return zeqSchema_; }
#endif

#ifdef BRAYNS_USE_DEFLECT
    /** Deflect parameters */
    /** DisplayCluster server host */
    std::string getDeflectHostname() const { return deflectHostname_; }
    /** Application stream name for DisplayCluster */
    std::string getDeflectStreamname() const { return deflectStreamname_; }
#endif

protected:

    std::string module_;
    std::string renderer_;
    std::string camera_;
    SceneEnvironment sceneEnvironment_;

    std::string swcFolder_;
    std::string pdbFolder_;
    std::string h5Folder_;
    std::string meshFolder_;
    size_t windowWidth_;
    size_t windowHeight_;

#ifdef BRAYNS_USE_RESTBRIDGE
    std::string restHostname_;
    size_t      restPort_;
    std::string zeqSchema_;
#endif
#ifdef BRAYNS_USE_DEFLECT
    std::string deflectHostname_;
    std::string deflectStreamname_;
#endif
};

}

#endif // APPLICATIONPARAMETERS_H
