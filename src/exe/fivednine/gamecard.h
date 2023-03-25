#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <fivednine/render/mesh.h>
#include <fivednine/render/shader.h>
#include <fivednine/render/texture.h>

class GameCard
{
public:
    GameCard(fivednine::render::ShaderPtr spShader);

    void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

    void SetPosition(float x, float y, float z);
    void SetDimensions(float width, float height);

    fivednine::render::TexturePtr GetTexture() const;
    void SetTexture(fivednine::render::TexturePtr spTexture);
    void SetUniformValue1f(const char* pUniformName, float* pValue);

private:
    GameCard() = delete;
    GameCard(const GameCard& other) = delete;

    fivednine::render::Mesh                          m_cardMesh;
    std::vector<fivednine::render::MeshUniformValue> m_uniformValues;

    // All game cards are fundamentally textured unit quads.
    // TODO: instanced rendering for game cards
    static const glm::vec3 s_Vertices[4];
    static const glm::vec2 s_UVs[4];
    static const int       s_Indices[6];
};

typedef std::shared_ptr<GameCard> GameCardPtr;