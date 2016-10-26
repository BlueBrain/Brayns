/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BRAYNSVIEWER_H
#define BRAYNSVIEWER_H

#include <apps/ui/BaseWindow.h>

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

private:

    virtual void _registerKeyboardShortcuts() final;
    void _gradientMaterials();
    void _pastelMaterials();
    void _randomMaterials();
    void _toggleIncrementalTimestamp();
    void _defaultTimestamp();

    float _timestampIncrement;
};

}
#endif // BRAYNSVIEWER_H
