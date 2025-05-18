#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <epoxy/gl.h> 

class ShaderManager {
    public:
        ShaderManager(std::string baseShaderPath = "");
        GLuint create_shader_program(const std::string& vertex_file, const std::string& fragment_file);
    
    private:
        
        std::string m_baseShaderPath;
        
        GLuint compile_shader(GLenum type, const std::string& source);
        std::string getShaderPath(const std::string& filename);
        std::string read_file(const std::string& filename);
        std::string getExecutableDir();
};

#endif