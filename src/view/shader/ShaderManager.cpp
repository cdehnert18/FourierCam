#include "ShaderManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <epoxy/gl.h>
#include <unistd.h>

ShaderManager::ShaderManager(std::string baseShaderPath) : m_baseShaderPath(std::move(baseShaderPath)) {}

// Reads the entire contents of a text file and returns it as a string
std::string ShaderManager::read_file(const std::string& filename) {
    auto filepath = getShaderPath(filename);
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open shader file: " << filepath << std::endl;
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compiles a shader from source code
GLuint ShaderManager::compile_shader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src_cstr = source.c_str();
    glShaderSource(shader, 1, &src_cstr, nullptr);
    glCompileShader(shader);

    // Check compile status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(log_length, ' ');
        glGetShaderInfoLog(shader, log_length, nullptr, &log[0]);
        std::cerr << "Shader compilation failed:\n" << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Creates a shader program from vertex and fragment shader source strings
GLuint ShaderManager::create_shader_program(const std::string& vertex_file, const std::string& fragment_file) {
    auto vertex_source = read_file(vertex_file);
    auto fragment_source = read_file(fragment_file);
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    if (vertex_shader == 0) return 0;

    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Shaders can be deleted after linking
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Check link status
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(log_length, ' ');
        glGetProgramInfoLog(program, log_length, nullptr, &log[0]);
        std::cerr << "Program linking failed:\n" << log << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

std::string ShaderManager::getShaderPath(const std::string& filename) {
    if (!m_baseShaderPath.empty()) {
        return (std::filesystem::path(m_baseShaderPath) / filename).string();
    }
    // Default path relative to executable
    return (std::filesystem::path(getExecutableDir()) / "view/shader/shaderfiles" / filename).string();
}

std::string ShaderManager::getExecutableDir() {
    char result[256];
    ssize_t count = readlink("/proc/self/exe", result, 256);
    std::string path = std::string(result, (count > 0) ? count : 0);
    return std::filesystem::path(path).parent_path();
}