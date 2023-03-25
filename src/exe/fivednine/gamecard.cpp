#include "gamecard.h"

// Vertices
// 3-----------------2
// |                 |
// |                 |
// |                 |
// |                 |
// |                 |
// |                 |
// 0-----------------1

const glm::vec3 GameCard::s_Vertices[4] = {
    glm::vec3(0.f, 0.f, 0.f),
    glm::vec3(1.f, 0.f, 0.f),
    glm::vec3(1.f, 1.f, 0.f),
    glm::vec3(0.f, 1.f, 0.f)
};

const glm::vec2 GameCard::s_UVs[4] = {
    glm::vec2(0.f, 0.f),
    glm::vec2(1.f, 0.f),
    glm::vec2(1.f, 1.f),
    glm::vec2(0.f, 1.f)
};

// Assuming GL_CCW winding order
const int GameCard::s_Indices[6] = {
    // Upper-right triangle
    0, 1, 2,
    // Lower-left triangle
    0, 2, 3
};

static constexpr uint32_t kMaxNumVertices = 4;
static constexpr uint32_t kMaxNumIndices = 6;

using namespace fivednine;
using namespace fivednine::render;

GameCard::GameCard(ShaderPtr spShader)
    : m_cardMesh(kMaxNumVertices, kMaxNumIndices)
{
    m_cardMesh.SetModelMatrix(glm::mat4(1.f));

    m_cardMesh.SetShader(spShader);
    m_cardMesh.SetPositions(s_Vertices, 4);
    m_cardMesh.SetTextureCoordinates(s_UVs, 4);
    m_cardMesh.SetIndices(s_Indices, 6);
}

void GameCard::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
    m_cardMesh.SetMeshUniforms(m_uniformValues);
    m_cardMesh.Draw(projMatrix, viewMatrix);
}

void GameCard::SetPosition(float x, float y, float z)
{
    glm::mat4& modelMatrix = m_cardMesh.GetModelMatrix();
    modelMatrix[3] = glm::vec4(x, y, z, 1.f);
}

void GameCard::SetDimensions(float width, float height)
{
    // Diagonal *= scale vector (z-axis scale is unity)
    glm::mat4& modelMatrix = m_cardMesh.GetModelMatrix();
    modelMatrix[0][0] *= width;
    modelMatrix[1][1] *= height;
}

TexturePtr GameCard::GetTexture() const
{
    return m_cardMesh.GetTexture();
}

void GameCard::SetTexture(TexturePtr spTexture)
{
    m_cardMesh.SetTexture(spTexture);
}

void GameCard::SetUniformValue1f(const char* pUniformName, float* pValue)
{
    auto it = std::find_if(std::begin(m_uniformValues), std::end(m_uniformValues),
        [pUniformName](const MeshUniformValue& uniformValue) -> bool
        {
            return uniformValue.Name == pUniformName;
        });
    if (it == std::end(m_uniformValues))
    {
        m_uniformValues.emplace_back(pUniformName, UniformType::Float, pValue);
        return;
    }

    it->pValue = pValue;
}