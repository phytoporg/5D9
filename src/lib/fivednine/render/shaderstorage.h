// shaderstorage.h
//
// Entity for compiling and linking shader programs, then storing them for later use.

#pragma once

#include "shader.h"
#include <string>
#include <vector>
#include <cstdint>

namespace fivednine { namespace render {
    class ShaderStorage
    {
    public:
        ~ShaderStorage();

        bool
        AddShader(
            const std::string& vertexText,
            const std::string& fragmentText,
            const std::string& shaderName
            );

        ShaderPtr FindShaderByName(const std::string& shaderName) const;

    private:
        uint32_t CompileVertexShader(const std::string& source);
        uint32_t CompileFragmentShader(const std::string& source);

        // LinkShaders
        bool 
        LinkShaders(
            uint32_t programHandle,
            uint32_t vertexShaderHandle,
            uint32_t fragmentShaderHandle
            );
        std::vector<ShaderAttribute> PopulateAttributes(uint32_t programHandle);
        std::vector<ShaderUniform> PopulateUniforms(uint32_t programHandle);

        std::vector<ShaderPtr> m_storageVector;
    };
}}