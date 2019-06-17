/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "shader.h"
#include <FreeImage.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include <brayns/common/types.h>

#include <apps/BraynsIBL/brdf.fs.h>
#include <apps/BraynsIBL/brdf.vs.h>
#include <apps/BraynsIBL/cubemap.vs.h>
#include <apps/BraynsIBL/equirectangular_to_cubemap.fs.h>
#include <apps/BraynsIBL/irradiance_convolution.fs.h>
#include <apps/BraynsIBL/prefilter.fs.h>

namespace
{
void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                                // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

struct Image
{
    size_t width;
    size_t height;
    std::vector<float> data;
    bool operator!() const { return data.empty(); }
};

Image loadImage(const std::string& envMap)
{
    auto image = FreeImage_Load(FIF_HDR, envMap.c_str());
    if (!image)
        return {};

    FreeImage_FlipVertical(image);
    const auto width = FreeImage_GetWidth(image);
    const auto height = FreeImage_GetHeight(image);
    const auto bpp = FreeImage_GetBPP(image);
    const auto pitch = width * bpp / 8;
    std::vector<float> rawData(height * pitch / sizeof(float));
    FreeImage_ConvertToRawBits((BYTE*)rawData.data(), image, pitch, bpp,
                               FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK,
                               FI_RGBA_BLUE_MASK, TRUE);
    FreeImage_Unload(image);
    return {width, height, rawData};
}

FIBITMAP* toImage(const float* texture, const size_t width, const size_t height,
                  const bool toByte = false)
{
    if (toByte)
    {
        auto img = FreeImage_Allocate(width, height, 8 * 3);
        RGBQUAD color;
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                const auto val = &texture[(x * 3) + y * width * 3];
                color.rgbRed = std::pow(std::min(*val, 1.f), 1.f / 2.2f) * 255;
                color.rgbGreen =
                    std::pow(std::min(*(val + 1), 1.f), 1.f / 2.2f) * 255;
                color.rgbBlue =
                    std::pow(std::min(*(val + 2), 1.f), 1.f / 2.2f) * 255;
                FreeImage_SetPixelColor(img, x, y, &color);
            }
        }
        return img;
    }

    auto img = FreeImage_AllocateT(FIT_RGBF, width, height, 32 * 3);
    for (unsigned y = 0; y < height; y++)
    {
        float* dst_bits = (float*)FreeImage_GetScanLine(img, y);
        memcpy(dst_bits, texture, width * 3 * sizeof(float));
        texture += (width * 3);
    }
    return img;
}

void saveImage(FIBITMAP* img, std::string filename, const bool asPNG = false)
{
    filename += asPNG ? ".png" : ".hdr";
    FreeImage_Save(asPNG ? FIF_PNG : FIF_HDR, img, filename.c_str());
}

void saveTex2d(const unsigned int texture, const unsigned int width,
               const unsigned int height, const std::string& filename,
               const bool asPNG = false)
{
    std::vector<float> buffer(width * height * 3);

    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, buffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    auto img = toImage(buffer.data(), width, height);
    saveImage(img, filename);
    FreeImage_Unload(img);

    if (asPNG)
    {
        auto imgByte = toImage(buffer.data(), width, height, true);
        saveImage(imgByte, filename, asPNG);
        FreeImage_Unload(imgByte);
    }
}

void saveCubemap(const unsigned int cubemap, const unsigned int size,
                 const std::string& filename, const bool asPNG = false,
                 const unsigned int level = 0)
{
    FIBITMAP* img =
        FreeImage_AllocateT(FIT_RGBF, size * 6, size, 24 * sizeof(float));
    FIBITMAP* pngImg{nullptr};
    if (asPNG)
        pngImg = FreeImage_Allocate(size * 4, size * 3, 24);

    unsigned int offsetX[] = {size * 2,  // right
                              0,         // left
                              size,      // top
                              size,      // bottom
                              size,      // front
                              size * 3}; // back
    unsigned int offsetY[] = {size, size, 0, size * 2, size, size};

    std::vector<float> buffer(size * size * 3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (uint8_t i = 0; i < 6; ++i)
    {
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, GL_RGB,
                      GL_FLOAT, buffer.data());
        auto face = toImage(buffer.data(), size, size);
        FreeImage_FlipVertical(face);
        FreeImage_Paste(img, face, i * size, 0, 255);
        FreeImage_Unload(face);
        if (pngImg)
        {
            auto faceByte = toImage(buffer.data(), size, size, true);
            FreeImage_FlipVertical(faceByte);
            FreeImage_Paste(pngImg, faceByte, offsetX[i], offsetY[i], 255);
            FreeImage_Unload(faceByte);
        }
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    saveImage(img, filename);
    FreeImage_Unload(img);

    if (pngImg)
    {
        saveImage(pngImg, filename, asPNG);
        FreeImage_Unload(pngImg);
    }
}

template <typename T>
auto val(T* in)
{
    return po::value<T>(in)->default_value(*in);
}
}

int main(int argc, char** argv)
{
    std::string envMap;
    size_t cubeMapSize = 512;
    size_t irradianceSize = 512;
    size_t radianceSize = 512;
    size_t brdfSize = 512;
    float irradianceSampleDelta = 0.025f;
    size_t radianceSamples = 8 * 1024;
    unsigned int radianceMips = 5;
    bool asPNG = false;

    po::positional_options_description posArg;
    posArg.add("envmap", 1);
    po::options_description options;
    options.add_options() //
        ("envmap", po::value<std::string>(&envMap)->required(),
         "Source HDR equirectangular envmap")                           //
        ("cubemap", val(&cubeMapSize), "Cubemap size")                  //
        ("irradiance", val(&irradianceSize), "Irradiance cubemap size") //
        ("radiance", val(&radianceSize), "Radiance cubemap size")       //
        ("brdf", val(&brdfSize), "BRDF LUT size")                       //
        ("irradiance-delta", val(&irradianceSampleDelta),
         "Irradiance sample delta") //
        ("radiance-samples", val(&radianceSamples),
         "Number of radiance samples") //
        ("radiance-mips", val(&radianceMips),
         "Number of radiance mip levels") //
        ("png", po::bool_switch(&asPNG), "Export maps as PNG");
    options.add_options()("help", "Print this help");

    try
    {
        const auto parsedOptions = po::command_line_parser(argc, argv)
                                       .options(options)
                                       .positional(posArg)
                                       .run();
        po::variables_map vm;
        po::store(parsedOptions, vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << options << std::endl;
            return EXIT_SUCCESS;
        }
    }
    catch (const po::required_option& e)
    {
        std::cout << e.what() << "\n" << options << std::endl;
        return EXIT_FAILURE;
    }

    catch (const po::error& e)
    {
        std::cerr << "Failed to parse commandline: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    namespace fs = boost::filesystem;
    const auto path = fs::path(envMap).parent_path();
    const auto basename = (path / fs::basename(envMap)).string();

    const std::string irradianceMapFilename = basename + brayns::IRRADIANCE_MAP;
    const std::string radianceMapFilename = basename + brayns::RADIANCE_MAP;
    const std::string brdfLutFilename = basename + brayns::BRDF_LUT;

    // glfw: initialize and configure
    // ------------------------------
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        glfwErrorCallback(1, "Failed to initialize GLFW");
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // glfw window creation
    // --------------------
    GLFWwindow* window =
        glfwCreateWindow(1, 1, "ibl_offscreen", nullptr, nullptr);
    if (!window)
    {
        glfwErrorCallback(2, "Failed to create GLFW window");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GL_NO_ERROR)
    {
        glfwErrorCallback(3, "Failed to initialize GLEW");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter
    // map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // build and compile shaders
    // -------------------------
    Shader equirectangularToCubemapShader(cubemap_vs,
                                          equirectangular_to_cubemap_fs);
    Shader irradianceShader(cubemap_vs, irradiance_convolution_fs);
    Shader prefilterShader(cubemap_vs, prefilter_fs);
    Shader brdfShader(brdf_vs, brdf_fs);

    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMapSize,
                          cubeMapSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------

    const auto image = loadImage(envMap);
    if (!image)
    {
        std::string msg("Failed to load environment map " + envMap);
        glfwErrorCallback(4, msg.c_str());
        glfwTerminate();
        return EXIT_FAILURE;
    }

    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, image.width, image.height, 0,
                 GL_RGB, GL_FLOAT, image.data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (uint8_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     cubeMapSize, cubeMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap
                                              // sampling (combatting visible
                                              // dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6
    // cubemap face directions
    // -------------------------------------------------------------------------
    glm::mat4 captureProjection =
        glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f))};

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, cubeMapSize, cubeMapSize); // don't forget to configure the
                                                // viewport to the capture
                                                // dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint8_t i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible
    // dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance
    // scale.
    // -------------------------------------------------------------------------
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (uint8_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     irradianceSize, irradianceSize, 0, GL_RGB, GL_FLOAT,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize,
                          irradianceSize);

    // pbr: solve diffuse integral by convolution to create an irradiance
    // (cube)map.
    // -------------------------------------------------------------------------
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    irradianceShader.setFloat("sampleDelta", irradianceSampleDelta);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, irradianceSize, irradianceSize); // don't forget to
                                                      // configure the viewport
                                                      // to the capture
                                                      // dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint8_t i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    saveCubemap(irradianceMap, irradianceSize, irradianceMapFilename, asPNG);
    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter
    // scale.
    // -------------------------------------------------------------------------
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (uint8_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     radianceSize, radianceSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // be sure to set minification  filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the
    // required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to
    // create a prefilter (cube)map.
    // -------------------------------------------------------------------------
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    prefilterShader.setFloat("resolution", cubeMapSize);
    prefilterShader.setUint("samples", radianceSamples);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint8_t mip = 0; mip < radianceMips; ++mip)
    {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth = radianceSize * std::pow(0.5, mip);
        unsigned int mipHeight = radianceSize * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
                              mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(radianceMips - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (uint8_t i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (uint8_t mip = 0; mip < radianceMips; ++mip)
    {
        saveCubemap(prefilterMap, radianceSize * std::pow(0.5f, mip),
                    radianceMapFilename +
                        (mip > 0 ? std::to_string((int)mip) : ""),
                    asPNG, mip);
    }

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, brdfSize, brdfSize, 0, GL_RG,
                 GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad
    // with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, brdfSize,
                          brdfSize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           brdfLUTTexture, 0);

    glViewport(0, 0, brdfSize, brdfSize);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    saveTex2d(brdfLUTTexture, brdfSize, brdfSize, brdfLutFilename, asPNG);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return EXIT_SUCCESS;
}
