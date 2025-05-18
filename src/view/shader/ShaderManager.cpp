#include "ShaderManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <epoxy/gl.h>  // or <GL/gl.h> depending on your setup
#include <unistd.h>

std::string getShaderPath(const std::string& filename) {
    return std::filesystem::path(getExecutableDir()) / "view/shader/shaderfiles/" / filename;
}

// Reads the entire contents of a text file and returns it as a string
std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open shader file: " << filename << std::endl;
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compiles a shader from source code
GLuint compile_shader(GLenum type, const std::string& source) {
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
GLuint create_shader_program(const std::string& vertex_source, const std::string& fragment_source) {
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

std::string getExecutableDir() {
    char result[256];
    ssize_t count = readlink("/proc/self/exe", result, 256);
    std::string path = std::string(result, (count > 0) ? count : 0);
    return std::filesystem::path(path).parent_path();
}