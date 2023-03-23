#include "gamecard.h"
#include <glm/gtc/matrix_transform.hpp>

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

using namespace countdown;
using namespace countdown::render;

GameCard::GameCard(ShaderPtr spShader, const glm::vec3& upperLeft, uint32_t width, uint32_t height)
    : m_cardMesh(4, 6)
{
    // By default each card's basic geometry is identical and represents a unit quad; use the 
    // model matrix to translate and scale to achieve changes to position and proportions.
    glm::mat4 modelMatrix(1.f);

    // Diagonal *= scale vector (z-axis scale is unity)
    modelMatrix[0][0] *= static_cast<float>(width);
    modelMatrix[1][1] *= static_cast<float>(height);
    modelMatrix[3] = glm::vec4(upperLeft, 1.f);

    m_cardMesh.SetPositions(s_Vertices, 4);
    m_cardMesh.SetTextureCoordinates(s_UVs, 4);
    m_cardMesh.SetIndices(s_Indices, 6);
    m_cardMesh.SetShader(spShader);
    m_cardMesh.SetModelMatrix(modelMatrix);
}

void GameCard::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
    m_cardMesh.Draw(projMatrix, viewMatrix);
}