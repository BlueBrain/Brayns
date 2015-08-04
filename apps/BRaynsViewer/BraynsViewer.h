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

    BraynsViewer( const ApplicationParameters& applicationParameters );

    /** Sets the rendering parameters and renders the current frame
     */
    void display();

    /** Creates intial materials with random values
     */
    void createMaterials();

    /** Loads data specified in the command line arguments. This includes
     * SWC, H5, PDB and all mesh files supported by the assimp library
     */
    void loadData();

    /** Builds the OSPRay specific geometry from loaded data
     */
    void buildGeometry();

    /** Builds a scene environment around the loaded data (e.g. Ground,
     * Box, etc.)
     *
     * @param scale Scales factor applied to geometry bounding box
     */
    void buildEnvironment( const ospray::vec3f& scale );


    /** Reshapes the current window with specified new size
     *
     * @param newSize new window size in pixels
     */
    void reshape(const ospray::vec2i& newSize);

    /** Handles keyboard interaction
     *
     * @param key key pressed
     * @param where location of the window cursor
     */
    void keypress(char key, const ospray::vec2f where);

private:
    std::string rendererType_;
};

}
#endif // BRAYNSVIEWER_H
