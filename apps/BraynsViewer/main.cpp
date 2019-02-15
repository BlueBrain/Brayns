/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include <apps/ui/Application.h>
#include <brayns/Brayns.h>
#include <brayns/common/log.h>

#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/ParametersManager.h>

#include <deps/glfw/include/GLFW/glfw3.h>

#include <cstdlib>

Application* appInstance = nullptr;

void cleanup()
{
    delete appInstance;
}

static void errorCallback(int error, const char* description)
{
    std::cerr << "Error: " << error << ": " << description << std::endl;
}

static void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/,
                        int action, int /*mods*/)
{
    appInstance->keyCallback(key, action);
}

static void cursorCallback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    appInstance->cursorCallback(xpos, ypos);
}

static void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action,
                                int /*mods*/)
{
    appInstance->mouseButtonCallback(button, action);
}

static void scrollCallback(GLFWwindow* /*window*/, double xoffset,
                           double yoffset)
{
    appInstance->scrollCallback(xoffset, yoffset);
}

int run(brayns::Brayns& brayns)
{
    int windowWidth = 800;
    int windowHeight = 600;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        errorCallback(1, "GLFW failed to initialize.");
        return EXIT_FAILURE;
    }

    const std::string windowTitle =
        "Brayns Viewer [" +
        brayns.getParametersManager().getApplicationParameters().getEngine() +
        "] ";

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight,
                                          windowTitle.c_str(), NULL, NULL);
    if (!window)
    {
        errorCallback(2, "glfwCreateWindow() failed.");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GL_NO_ERROR)
    {
        errorCallback(3, "GLEW failed to initialize.");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    appInstance = new Application(brayns, window, windowWidth, windowHeight);
    auto& g_app = *appInstance;

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);

    std::array<int, 2> windowPos;
    std::array<int, 2> windowSizePrev;
    bool fullscreen = false;

    const auto toggleFullscreen = [&]() {
        const bool fullscreenCurr = g_app.isFullScreen();

        if (fullscreen != fullscreenCurr)
        {
            if (fullscreenCurr)
            {
                // backup window position and window size
                glfwGetWindowPos(window, &windowPos[0], &windowPos[1]);
                glfwGetWindowSize(window, &windowSizePrev[0],
                                  &windowSizePrev[1]);

                auto monitor = glfwGetPrimaryMonitor();

                // get reolution of monitor
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);

                // switch to full screen
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width,
                                     mode->height, 0);
            }
            else
            {
                // restore last window size and position
                glfwSetWindowMonitor(window, nullptr, windowPos[0],
                                     windowPos[1], windowSizePrev[0],
                                     windowSizePrev[1], 0);
            }

            fullscreen = fullscreenCurr;
        }
    };

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        g_app.timerEnd();
        g_app.timerBegin();

        if (g_app.exitCalled())
            break;

        toggleFullscreen();

        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        g_app.reshape(windowWidth, windowHeight);

        g_app.render();
        g_app.guiNewFrame();
        g_app.guiRender();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_app.guiShutdown();

    glfwTerminate();

    return EXIT_SUCCESS;
}

int main(int argc, const char** argv)
{
    try
    {
        brayns::Brayns brayns(argc, argv);
        atexit(cleanup);
        run(brayns);
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
