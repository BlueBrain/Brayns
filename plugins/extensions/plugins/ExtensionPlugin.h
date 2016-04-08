/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef EXTENSIONPLUGIN_H
#define EXTENSIONPLUGIN_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/common/parameters/ApplicationParameters.h>

namespace brayns
{

/**
   Defines the abstract representation of a extension plug-in. What we mean by
   extension is a set a functionalities that are not provided by the core of
   the application. For example, exposing a REST interface via HTTP, or
   streaming images to an distant display.
 */
class ExtensionPlugin
{
public:
    virtual ~ExtensionPlugin() {}

    /**
        Executes the core functionnalities of the plugin and modifies the
        ExtensionParameters accordingly
    */
    BRAYNS_API virtual void run( ) = 0;

protected:
    ExtensionPlugin( ApplicationParameters&, ExtensionParameters& );

    ApplicationParameters& _applicationParameters;
    ExtensionParameters& _extensionParameters;
};

}
#endif // EXTENSIONPLUGIN_H
