/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#pragma once

#include <brayns/engine/Engine.h>
#include <brayns/io/LoaderRegistry.h>
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
/**
    Brayns is a minimalistic library that allows optimized ray-tracing rendering
    of meshes and parametric geometry. Brayns provides an abstraction of the
    underlying rendering engines, making it possible to use the best rendering
    engine depending on the case.

    Brayns uses plugins for extended function. There are a few built-in plugins
    and additional plugins can be dynamically loaded.

    The underlying rendering engine (OSPRay) is specified
    in the rendering parameters and is invoked by the render method for
    generating the frames.

    Underlying rendering engines support CPU, GPU and heterogeneous
    architectures

    This object exposes the basic API for Brayns
*/
class Brayns
{
public:
    /** Brayns instance initialization.
     *
     * Initialization involves command line parsing, engine creation, plugin
     * loading and initialization, data loading and scene creation.
     *
     * In a setup using event loops, the event loop must be set up correctly
     * before calling this constructor to ensure that plugins can install their
     * event callbacks successfully.
     *
     * Command line parameters provide options about the application itself,
     * the geometry and the renderer. Brayns creates the scene using built-in
     * and plug-in provided loaders.
     */
    Brayns(int argc, const char** argv);
    ~Brayns();

    /**
     * Renders color and depth buffers of the current scene, according to
     * default parameters. This is typically used by an application that does
     * not provide any on-screen visualization. In such cases, input and output
     * parameters are provided by network events. For instance, a camera event
     * defines the origin, target and up vector of the camera, and an ImageJPEG
     * event triggers the rendering and gathers the results in a form of a
     * base64 encoded JPEG image.
     *
     * Combines commit(), render() and postRender() together in a synchronized
     * fashion.
     *
     * @return true if rendering should continue or false if user inputs
     *         requested to stop.
     */
    bool commitAndRender();
    //@}

    /** @name Low-level execution API */
    //@{
    /**
     * Handle events, update animation, call preRender() on plugins and commit
     * changes on the engine, scene, camera, renderer, etc. to prepare rendering
     * of a new frame.
     *
     * @return true if render() is allowed/needed after all states have been
     *         evaluated (accum rendering, data loading, etc.)
     * @note threadsafe with render()
     */
    bool commit();

    /**
     * Render a frame into the current framebuffer.
     * @note threadsafe with commit()
     */
    void render();

    /**
     * Call postRender() on engine and plugins to signal finish of render().
     * Shall only be called after render() has finished. This is only needed if
     * commit() and render() are called individually.
     */
    void postRender();
    //@}

    /**
       @return the current engine
    */
    Engine& getEngine();

    /**
     * @return The parameter manager
     */
    ParametersManager& getParametersManager();

    /**
     * @brief getLoaderRegistry gives access to the loaders registry
     * @return LoaderRegistry&
     */
    LoaderRegistry& getLoaderRegistry();

    /**
     * @brief Get the registered network interface.
     *
     * @return ActionInterface* Network interface or null if not set.
     */
    ActionInterface* getActionInterface();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};
} // namespace brayns
