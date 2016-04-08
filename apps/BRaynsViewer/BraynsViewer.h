/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef BRAYNSVIEWER_H
#define BRAYNSVIEWER_H

#include <brayns/common/ui/BaseWindow.h>

namespace brayns
{

class DeflectManager;

class BraynsViewer : public BaseWindow
{
public:

    BraynsViewer( BraynsPtr brayns, int argc, const char **argv );

    /** Sets the rendering parameters and renders the current frame
     */
    void display();

    /** Handles keyboard interaction
     * '4' : Creates gradient materials from red to yellow
     * '5' : Creates random pastel-colored materials
     * '6' : Creates random materials including reflection and transparency
     * '7' : Creates shades of grey materials
     * 'g' : Enables/Disables auto-incrementation of scene timestamp
     * '[' : Moves scene timestamp backwards by 1 unit
     * ']' : Moves scene timestamp forward by 1 unit
     * '*' : Displays parameters helper in the console
     *
     * @param key key pressed
     * @param where location of the window cursor
     */
    void keypress(char key, const Vector2f& where) final;

private:
    float _timestampIncrement;
};

}
#endif // BRAYNSVIEWER_H
