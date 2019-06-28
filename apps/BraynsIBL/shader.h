#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>

class Shader
{
public:
    Shader(const char *vShaderCode, const char *fShaderCode)
    {
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        _id = glCreateProgram();
        glAttachShader(_id, vertex);
        glAttachShader(_id, fragment);
        glLinkProgram(_id);
        checkCompileErrors(_id, "PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    void use() { glUseProgram(_id); }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
    }
    void setUint(const std::string &name, uint value) const
    {
        glUniform1ui(glGetUniformLocation(_id, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE,
                           &mat[0][0]);
    }

private:
    unsigned int _id;
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
                          << "\n"
                          << infoLog << "\n -- "
                                        "--------------------------------------"
                                        "------------- -- "
                          << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
                          << "\n"
                          << infoLog << "\n -- "
                                        "--------------------------------------"
                                        "------------- -- "
                          << std::endl;
            }
        }
    }
};
