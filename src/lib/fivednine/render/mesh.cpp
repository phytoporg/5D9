#include "mesh.h"
#include "rendercommon.h"
#include "uniform.h"
#include "attribute.h"
#include "indexbuffer.h"
#include "draw.h"

#include <fivednine/render/uniform.h>
#include <fivednine/log/log.h>

using namespace fivednine::render;

void Mesh::CopyTo(Mesh& other) const
{
    other.m_VertexPositionAttribute = m_VertexPositionAttribute;
    other.m_VertexTextureCoordinateAttribute = m_VertexTextureCoordinateAttribute;
    other.m_Indices = m_Indices;

    other.m_VertexPositions = m_VertexPositions;
    other.m_VertexTextureCoordinates = m_VertexTextureCoordinates;
    other.m_VertexIndices = other.m_VertexIndices;

    other.m_MeshUniformValues = m_MeshUniformValues;

    other.m_spShader = m_spShader;

    other.m_VAO = m_VAO;

    other.m_ModelUniformHandle = m_ModelUniformHandle;
    other.m_ViewUniformHandle = m_ViewUniformHandle;
    other.m_ProjUniformHandle = m_ProjUniformHandle;

    other.m_isVisible = m_isVisible;

    other.m_isDirty   = true;
}

void Mesh::BindAttributes()
{
    m_VertexPositionAttribute.BindTo(0);
    m_VertexTextureCoordinateAttribute.BindTo(1);
}

void Mesh::UnbindAttributes()
{
    m_VertexPositionAttribute.UnbindFrom(0);
    m_VertexTextureCoordinateAttribute.UnbindFrom(1);
}

Mesh::Mesh(size_t maxVertexCount, size_t maxIndexCount) 
{
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    m_VertexPositionAttribute.Set(m_VertexPositions.data(), maxVertexCount);
    m_VertexPositionAttribute.BindTo(0);

    m_VertexTextureCoordinateAttribute.Set(m_VertexTextureCoordinates.data(), maxVertexCount);
    m_VertexTextureCoordinateAttribute.BindTo(1);

    m_Indices.Set(m_VertexIndices.data(), maxIndexCount);

    m_VAO = vao;

    glBindVertexArray(0);
}

Mesh::Mesh(const Mesh& other)
{
    // Deep copy
    other.CopyTo(*this);
}

Mesh& Mesh::operator=(const Mesh& other)
{
    other.CopyTo(*this);
    m_isDirty = true;
    return *this;
}

bool Mesh::SetPositions(const glm::vec3* positionsArray, uint32_t arrayLen)
{
    // Populate vertex array
    m_VertexPositions.resize(arrayLen);
    for (uint32_t i = 0; i < arrayLen; ++i)
    {
        m_VertexPositions[i] = positionsArray[i];
    }

    m_VertexPositionAttribute.Set(m_VertexPositions.data(), m_VertexPositions.size());
    m_isDirty = true;
    return true;
}

bool Mesh::SetTextureCoordinates(const glm::vec2* uvsArray, uint32_t arrayLen)
{
    // Convert to floating-point representation for GPU
    m_VertexTextureCoordinates.resize(arrayLen);
    for (size_t i = 0; i < arrayLen; ++i)
    {
        m_VertexTextureCoordinates[i] = uvsArray[i];
    }

    m_VertexTextureCoordinateAttribute.Set(m_VertexTextureCoordinates.data(), m_VertexTextureCoordinates.size());
    m_isDirty = true;
    return true;
}

bool Mesh::SetIndices(const int* indexArray, uint32_t arrayLen)
{    
    m_VertexIndices.resize(arrayLen);
    memcpy(m_VertexIndices.data(), indexArray, arrayLen * sizeof(int));
    m_Indices.Set(m_VertexIndices.data(), m_VertexIndices.size());
    m_isDirty = true;
    return true;
}

bool Mesh::SetShader(ShaderPtr spShader)
{
    if (!spShader)
    {
        return false;
    }
    
    m_spShader = spShader;
    m_ModelUniformHandle = spShader->GetUniform("model");
    if (m_ModelUniformHandle == Shader::kInvalidHandleValue)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_RENDER,
            "Could not retrieve expected uniform 'model' from shader '%s'",
            spShader->GetName().c_str());
        return false;
    }

    m_ViewUniformHandle = spShader->GetUniform("view");
    if (m_ViewUniformHandle == Shader::kInvalidHandleValue)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_RENDER,
           "Could not retrieve expected uniform 'view' from shader '%s'",
           spShader->GetName().c_str());
        return false;
    }

    m_ProjUniformHandle = spShader->GetUniform("projection");
    if (m_ProjUniformHandle == Shader::kInvalidHandleValue)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_RENDER,
            "Could not retrieve expected uniform 'projection' from shader '%s'",
            spShader->GetName().c_str());
        return false;
    }

    m_ProjUniformHandle = spShader->GetUniform("projection");
    if (m_ProjUniformHandle == Shader::kInvalidHandleValue)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_RENDER,
            "Could not retrieve expected uniform 'projection' from shader '%s'",
            spShader->GetName().c_str());
        return false;
    }

    m_SamplerUniformHandle = spShader->GetUniform("sampler");
    if (m_SamplerUniformHandle == Shader::kInvalidHandleValue)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_RENDER,
            "Could not retrieve expected uniform 'sampler' from shader '%s'",
            spShader->GetName().c_str());
        return false;
    }

    return true;
}

void Mesh::SetModelMatrix(const glm::mat4& modelMatrix) 
{
    m_ModelMatrix = modelMatrix;
}

glm::mat4& Mesh::GetModelMatrix()
{
    return m_ModelMatrix;
}

void Mesh::SetTexture(TexturePtr spTexture)
{
    m_spTexture = spTexture;
}

TexturePtr Mesh::GetTexture() const
{
    return m_spTexture;
}

void Mesh::SetMeshUniforms(const std::initializer_list<MeshUniformValue>& uniformValues)
{
    m_MeshUniformValues.clear();
    for (const MeshUniformValue& meshUniformValue : uniformValues)
    {
        m_MeshUniformValues.emplace_back(meshUniformValue);
    }
}

void Mesh::SetMeshUniforms(const std::vector<MeshUniformValue>& uniformValues)
{
    m_MeshUniformValues = uniformValues;
}

void Mesh::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix) 
{
    if (!GetVisibility())
    {
        // Nothing to do if we're not visible
        return;
    }
    
    if (!m_spShader || 
         m_ModelUniformHandle == Shader::kInvalidHandleValue ||
         m_ViewUniformHandle  == Shader::kInvalidHandleValue ||
         m_ProjUniformHandle  == Shader::kInvalidHandleValue)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_RENDER,
            "Attempting to render mesh with an improper or invalid shader.");
        return;
    }

    m_spShader->Bind();

    for (size_t i = 0; i < m_MeshUniformValues.size(); ++i)
    {
        MeshUniformValue& uniformValue = m_MeshUniformValues[i];
        if (uniformValue.Location == Shader::kInvalidHandleValue)
        {
            uniformValue.Location = m_spShader->GetUniform(uniformValue.Name);
        }

        if (uniformValue.Location != Shader::kInvalidHandleValue)
        {
            SetUniformByType(uniformValue.Location, uniformValue.Type, uniformValue.pValue);
        }
        else
        {
            RELEASE_LOGLINE_ERROR(
                LOG_RENDER,
                "Failed to retrieve a location for uniform named '%s' in shader '%s'",
                uniformValue.Name, m_spShader->GetName().c_str());
        }
    }

    Uniform<glm::mat4>::Set(m_ModelUniformHandle, m_ModelMatrix);
    Uniform<glm::mat4>::Set(m_ViewUniformHandle, viewMatrix);
    Uniform<glm::mat4>::Set(m_ProjUniformHandle, projMatrix);

    if (m_spTexture)
    {
        Uniform<int>::Set(m_SamplerUniformHandle, 0);
        m_spTexture->Bind();
    }

    glBindVertexArray(m_VAO);

    BindAttributes();

    if (m_isDirty)
    {
        m_VertexPositionAttribute.Set(m_VertexPositions.data(), m_VertexPositions.size());
        m_VertexTextureCoordinateAttribute.Set(m_VertexTextureCoordinates.data(), m_VertexTextureCoordinates.size());
        m_Indices.Set(m_VertexIndices.data(), m_VertexIndices.size());
        m_isDirty = false;
    }

    ::Draw(m_Indices, DrawMode::Triangles);
    UnbindAttributes();

    m_spShader->Unbind();

    glBindVertexArray(0);

    if (m_spTexture)
    {
        m_spTexture->Unbind();
    }
}

void Mesh::SetVisibility(bool newVisibility)
{
    m_isVisible = newVisibility;
}

bool Mesh::GetVisibility() const
{
    return m_isVisible;
}

void Mesh::MarkDirty()
{
    m_isDirty = true;
}