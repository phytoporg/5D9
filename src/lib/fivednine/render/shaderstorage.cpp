#include "shaderstorage.h"
#include "rendercommon.h"

#include <fivednine/log/log.h>

#include <algorithm>
#include <cstring>

using namespace fivednine;
using namespace fivednine::render;

ShaderStorage::~ShaderStorage()
{
    m_storageVector.clear();
}

bool
ShaderStorage::AddShader(
    const std::string& vertexText,
    const std::string& fragmentText,
    const std::string& shaderName
    )
{
    const std::string vertexShaderSource = vertexText.c_str();
    if (vertexShaderSource.empty())
    {
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Could not load vertex shader for %s", shaderName.c_str());
        return false;
    }

    const std::string fragmentShaderSource = fragmentText.c_str();
    if (fragmentShaderSource.empty())
    {
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Could not load fragment shader for %s", shaderName.c_str());
        return false;
    }

    const uint32_t vertexShaderHandle = CompileVertexShader(vertexShaderSource);
    if (!vertexShaderHandle) { return false; }

    const uint32_t fragmentShaderHandle = CompileFragmentShader(fragmentShaderSource);
    if (!fragmentShaderHandle) { return false; }

    const uint32_t programHandle = glCreateProgram();
    if (!LinkShaders(programHandle, vertexShaderHandle, fragmentShaderHandle))
    {
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Failed to link shader %s", shaderName.c_str());
        return false;
    }

    const std::vector<ShaderAttribute> attributes = PopulateAttributes(programHandle);
    const std::vector<ShaderUniform> uniforms = PopulateUniforms(programHandle);

    Shader* pShader = new Shader(programHandle, attributes, uniforms, shaderName);
    if (!pShader)
    {
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Failed to allocate shader. OOM?");
        return false;
    }

    if (FindShaderByName(pShader->GetName()))
    {
        RELEASE_LOGLINE_WARNING(LOG_RENDER, "Attempted to add duplicate shader: %s", pShader->GetName().c_str());
        delete pShader;
        return false;
    }
     
    m_storageVector.emplace_back(pShader);
    return true;
}

ShaderPtr ShaderStorage::FindShaderByName(const std::string& shaderName) const 
{
    for (const ShaderPtr& spShader : m_storageVector)
    {
        if (spShader->GetName() == shaderName)
        {
            return spShader;
        }
    }

    return nullptr;
}

uint32_t ShaderStorage::CompileVertexShader(const std::string& source) 
{
    uint32_t shaderHandle = glCreateShader(GL_VERTEX_SHADER);
    const char* pSource = source.c_str();
    glShaderSource(shaderHandle, 1, &pSource, NULL);
    glCompileShader(shaderHandle);

    int success = 0;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderHandle, sizeof(infoLog), NULL, infoLog);
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Vertex shader compilation failed (%s)\n\t%s", infoLog);
        glDeleteProgram(shaderHandle);
        return 0;
    }

    return shaderHandle;
}

uint32_t ShaderStorage::CompileFragmentShader(const std::string& source) 
{
    uint32_t shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    const char* pSource = source.c_str();
    glShaderSource(shaderHandle, 1, &pSource, NULL);
    glCompileShader(shaderHandle);

    int success = 0;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderHandle, sizeof(infoLog), NULL, infoLog);
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Fragment shader compilation failed\n\t%s", infoLog);
        glDeleteProgram(shaderHandle);
        return 0;
    }

    return shaderHandle;
}

bool ShaderStorage::LinkShaders(uint32_t programHandle, uint32_t vertexShaderHandle, uint32_t fragmentShaderHandle) 
{
    if (vertexShaderHandle <= 0 || fragmentShaderHandle <= 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Invalid shader handle passed to LinkShaders()");
        return false;
    }

    glAttachShader(programHandle, vertexShaderHandle);
    glAttachShader(programHandle, fragmentShaderHandle);
    glLinkProgram(programHandle);

    int success = 0;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(programHandle, sizeof(infoLog), NULL, infoLog);
        RELEASE_LOGLINE_ERROR(LOG_RENDER, "Failed to link shader \t%s", infoLog);
        glDeleteShader(vertexShaderHandle);
        glDeleteShader(fragmentShaderHandle);
        return false;
    }

    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    return true;   
}

std::vector<ShaderAttribute> ShaderStorage::PopulateAttributes(uint32_t programHandle) 
{
    int count = -1;
    int length = -1;
    int size = -1;
    char name[128];
    GLenum type;

    glUseProgram(programHandle);
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &count);

    std::vector<ShaderAttribute> attributes;
    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(name));
        glGetActiveAttrib(programHandle, static_cast<GLuint>(i), sizeof(name), &length, &size, &type, name);
        const int attributeLocation = glGetAttribLocation(programHandle, name);
        if (attributeLocation >= 0)
        {
            attributes.emplace_back(name, attributeLocation);
        }
    }

    glUseProgram(0);
    return attributes;
}

std::vector<ShaderUniform> ShaderStorage::PopulateUniforms(uint32_t programHandle) 
{
    int count = -1;
    int length = -1;
    int size = -1;
    char name[128];
    GLenum type;

    glUseProgram(programHandle);
    glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &count);

    std::vector<ShaderUniform> uniforms;
    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(name));
        glGetActiveUniform(programHandle, static_cast<GLuint>(i), sizeof(name), &length, &size, &type, name);
        const int uniformLocation = glGetUniformLocation(programHandle, name);
        if (uniformLocation >= 0)
        {
            std::string uniformName = name;
            const size_t leftBracketIndex = uniformName.find('[');
            if (leftBracketIndex != std::string::npos)
            {
                // This uniform is an array. Append [index] for every valid index in the uniform to
                // build the name;

                char uniformArrayElementName[256];
                uniformName.erase(std::begin(uniformName) + leftBracketIndex, std::end(uniformName));
                uint32_t uniformIndex = 0;
                uint32_t uniformLocation = 0;

                while(true)
                {
                    memset(uniformArrayElementName, 0, sizeof(uniformArrayElementName));
                    sprintf(uniformArrayElementName, "%s[%d]", uniformName.c_str(), uniformIndex++);

                    uniformLocation = glGetUniformLocation(programHandle, uniformArrayElementName);
                    if (uniformLocation >= 0)
                    {
                        uniforms.emplace_back(uniformArrayElementName, uniformLocation);
                    }
                    else
                    {
                        // Reached the end of the uniform array.
                        break;
                    }
                }
            }
            uniforms.emplace_back(uniformName, uniformLocation);
        }
    }

    glUseProgram(0);
    return uniforms;
}