#include "stdafx.h"
#include "glutils.h"

namespace Shader {
    uint buildShader(std::string const& filename, GLenum shaderType) {
        uint shader = glCreateShader(shaderType);
        std::ifstream fs(filename);
        if (!fs.is_open()) {
            throw Exception("Failed to open shader file" + filename);
        }
        std::ostringstream ss;
        ss << fs.rdbuf();
        fs.close();
        std::string source = ss.str();
        const char* c_str = source.c_str();
        glShaderSource(shader, 1, &c_str, NULL);

        glCompileShader(shader);
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << infoLog << std::endl;
            throw Exception("Failed to compile shader " + filename);
        }
        return shader;
    }
}

uint Shader::buildVertexShader(std::string const& filename) {
    return buildShader(filename, GL_VERTEX_SHADER);
}

uint Shader::buildGeometryShader(std::string const& filename) {
    return buildShader(filename, GL_GEOMETRY_SHADER);
}

uint Shader::buildFragShader(std::string const& filename) {
    return buildShader(filename, GL_FRAGMENT_SHADER);
}

void Shader::deleteShader(uint shader) {
    glDeleteShader(shader);
}

ShaderProgram::ShaderProgram() : ID(glCreateProgram()) {}

ShaderProgram::ShaderProgram(std::string const& vertPath, std::string const& fragPath) : ShaderProgram() {
    setVertexShader(vertPath);
    setFragShader(fragPath);
    link();
}

ShaderProgram::ShaderProgram(std::string const& vertPath, std::string const& geomPath, std::string const& fragPath) : ShaderProgram() {
    setVertexShader(vertPath);
    setGeometryShader(geomPath);
    setFragShader(fragPath);
    link();
}

ShaderProgram::~ShaderProgram() {
    for (uint shader : exclusiveShaders) {
        glDeleteShader(shader);
    }
    glDeleteProgram(ID);
}

void ShaderProgram::setVertexShader(uint shader) {
    glAttachShader(ID, shader);
}

void ShaderProgram::setVertexShader(std::string const& filename) {
    uint shader = Shader::buildVertexShader(filename);
    exclusiveShaders.insert(shader);
    glAttachShader(ID, shader);
}

void ShaderProgram::setGeometryShader(uint shader) {
    glAttachShader(ID, shader);
}

void ShaderProgram::setGeometryShader(std::string const& filename) {
    uint shader = Shader::buildGeometryShader(filename);
    exclusiveShaders.insert(shader);
    glAttachShader(ID, shader);
}

void ShaderProgram::setFragShader(uint shader) {
    glAttachShader(ID, shader);
}

void ShaderProgram::setFragShader(std::string const& filename) {
    uint shader = Shader::buildFragShader(filename);
    exclusiveShaders.insert(shader);
    glAttachShader(ID, shader);
}

void ShaderProgram::link() {
    glLinkProgram(ID);

    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
        throw Exception("Failed to link shader program " + std::to_string(ID));
    }

    for (uint shader : exclusiveShaders) {
        glDeleteShader(shader);
    }
}

uint Texture::loadFromFile(std::string const& filename, bool RGBA) {
    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* textureData = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (textureData) {
        glTexImage2D(GL_TEXTURE_2D, 0, RGBA ? GL_RGBA : GL_RGB, width, height, 0, RGBA ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        throw Exception("Failed to load texture " + filename + "\nstbi_image: " + std::string(stbi_failure_reason()));
    }
    stbi_image_free(textureData);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
