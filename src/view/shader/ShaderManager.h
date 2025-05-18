#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <epoxy/gl.h> 

std::string getShaderPath(const std::string& filename);
std::string read_file(const std::string& filename);
GLuint compile_shader(GLenum type, const std::string& source);
GLuint create_shader_program(const std::string& vertex_source, const std::string& fragment_source);
std::string getExecutableDir();

#endif