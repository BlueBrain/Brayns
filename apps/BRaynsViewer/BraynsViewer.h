/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#ifndef BRAYNSVIEWER_H
#define BRAYNSVIEWER_H

#include <brayns/common/ui/BaseWindow.h>

namespace brayns
{

class DeflectManager;

class BraynsViewer : public BaseWindow
{
public:

    BraynsViewer( int argc, const char **argv );

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
