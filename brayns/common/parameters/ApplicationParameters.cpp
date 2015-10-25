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

#include "ApplicationParameters.h"
#include <brayns/common/exceptions.h>
#include <brayns/common/log.h>

namespace brayns
{

const std::string PARAM_MODULE           = "module";
const std::string PARAM_RENDERER         = "renderer";
const std::string PARAM_SWC_FOLDER       = "swc-folder";
const std::string PARAM_H5_FOLDER        = "h5-folder";
const std::string PARAM_PDB_FOLDER       = "pdb-folder";
const std::string PARAM_MESH_FOLDER      = "mesh-folder";
const std::string PARAM_BUFFER_WIDTH     = "buffer-width";
const std::string PARAM_BUFFER_HEIGHT    = "buffer-height";
const std::string PARAM_METABALLS        = "metaballs";
const std::string PARAM_STEREO_CAMERA    = "stereo-camera";
const std::string PARAM_MATERIAL_SCHEME  = "material-scheme";
const std::string PARAM_SCENE_ENVIRONMENT= "scene-environment";
const std::string PARAM_BENCHMARKING     = "enable-benchmark";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;

const std::string PARAM_DEFLECT_HOSTNAME   = "deflect-hostname";
const std::string DEFAULT_DEFLECT_HOSTNAME   = "localhost";
const std::string DEFAULT_DEFLECT_STREAMNAME = "brayns";

const std::string PARAM_ZEQ_SCHEMA  = "zeq-schema";

const std::string DEFAULT_RENDERER = "raycast_eyelight";
const std::string DEFAULT_CAMERA   = "perspective";

ApplicationParameters::ApplicationParameters()
  : renderer_(DEFAULT_RENDERER),
    camera_(DEFAULT_CAMERA),
    sceneEnvironment_(seNone),
    windowWidth_(DEFAULT_WINDOW_WIDTH),
    windowHeight_(DEFAULT_WINDOW_HEIGHT),
    deflectHostname_(DEFAULT_DEFLECT_HOSTNAME),
    deflectStreamname_(DEFAULT_DEFLECT_STREAMNAME),
    benchmarking_(false)
{
    parameters_[PARAM_MODULE] =
        {ptString, "Name of the OSPRay module"};
    parameters_[PARAM_RENDERER] =
        {ptString, "Name of the renderer"};
    parameters_[PARAM_SWC_FOLDER] =
        {ptString, "Folder containing SWC files"};
    parameters_[PARAM_H5_FOLDER] =
        {ptString, "Folder containing H5 files"};
    parameters_[PARAM_MESH_FOLDER] =
        {ptString, "Folder containing PARAM_BENCHMARKINGmeshes"};
    parameters_[PARAM_BUFFER_HEIGHT] =
        {ptInteger, "Height of rendering buffer"};
    parameters_[PARAM_BUFFER_WIDTH] =
        {ptInteger, "Width of rendering buffer"};
    parameters_[PARAM_METABALLS] =
        {ptBoolean, "Generates metaballs from morphologies"};
    parameters_[PARAM_STEREO_CAMERA] =
        {ptBoolean, "Generates metaballs from morphologies"};
    parameters_[PARAM_MATERIAL_SCHEME] =
        {ptInteger, "Material scheme for molecular visualization "\
         "(0: Atoms, 1: Chains, 2: Residues)"};
    parameters_[PARAM_SCENE_ENVIRONMENT] =
        {ptInteger, "Scene environment (0: none, 1: ground, 2: box)"};
    parameters_[PARAM_DEFLECT_HOSTNAME] =
        {ptString, "Name of host running DisplayCluster"};
    parameters_[DEFAULT_DEFLECT_STREAMNAME] =
        {ptString, "Name of DisplayCluster stream"};
    parameters_[PARAM_ZEQ_SCHEMA] =
        {ptString, "Schema name for ZeroEQ communication"};
    parameters_[PARAM_BENCHMARKING] =
        {ptString, "Activates application benchmarking"};
}

void ApplicationParameters::parse(int argc, const char **argv)
{
    // Save arguments for later use
    for (int i=0;i<argc;i++)
        arguments_.push_back(argv[i]);

    // Parse arguments and populate class members accordingly
    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        arg.erase(0,2);
        if(arg == PARAM_RENDERER)
            renderer_ = argv[++i];
        if(arg == PARAM_SWC_FOLDER)
            swcFolder_ = argv[++i];
        if (arg == PARAM_H5_FOLDER)
            h5Folder_ = argv[++i];
        if (arg == PARAM_PDB_FOLDER)
            pdbFolder_ = argv[++i];
        if (arg == PARAM_MESH_FOLDER)
            meshFolder_ = argv[++i];
        else if (arg == PARAM_MODULE)
            module_ = argv[++i];
        else if (arg == PARAM_STEREO_CAMERA)
            camera_ = "stereo";
        else if (arg == PARAM_BUFFER_WIDTH)
            windowWidth_ = atoi(argv[++i]);
        else if (arg == PARAM_BUFFER_HEIGHT)
            windowHeight_ = atoi(argv[++i]);
        else if (arg == PARAM_SCENE_ENVIRONMENT)
            sceneEnvironment_ = static_cast<SceneEnvironment>(atoi(argv[++i]));
        if(arg==PARAM_ZEQ_SCHEMA)
            zeqSchema_ = argv[++i];
        if(arg==PARAM_DEFLECT_HOSTNAME)
            deflectHostname_ = argv[++i];
        if(arg==DEFAULT_DEFLECT_STREAMNAME)
            deflectStreamname_ = argv[++i];
        if(arg==PARAM_BENCHMARKING)
            benchmarking_ = true;
    }
}

void ApplicationParameters::display() const
{
    BRAYNS_INFO << "Application options: " << std::endl;
    BRAYNS_INFO << "- OSPRay Module : " << module_ << std::endl;
    BRAYNS_INFO << "- Renderer      : " << renderer_ << std::endl;
    BRAYNS_INFO << "- SWC folder    : " << swcFolder_ << std::endl;
    BRAYNS_INFO << "- PDB folder    : " << pdbFolder_ << std::endl;
    BRAYNS_INFO << "- H5 folder     : " << h5Folder_ << std::endl;
    BRAYNS_INFO << "- Mesh folder   : " << meshFolder_ << std::endl;
    BRAYNS_INFO << "- Window size   : " << windowWidth_ << "x" <<
                                           windowHeight_ << std::endl;
    BRAYNS_INFO << "- Camera        : " << camera_ << std::endl;
    BRAYNS_INFO << "- Rest schema   : " << zeqSchema_ << std::endl;
    BRAYNS_INFO << "- Deflect       : " << deflectHostname_ << ":" <<
                                           deflectStreamname_ << std::endl;
    BRAYNS_INFO << "- Benchmarking  : " <<
                   (benchmarking_ ? "On" : "Off") << std::endl;
}

}
