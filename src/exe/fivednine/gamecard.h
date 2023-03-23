#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <fivednine/render/mesh.h>
#include <fivednine/render/shader.h>
#include <fivednine/render/texture.h>

class GameCard
{
public:
    GameCard(fivednine::render::ShaderPtr spShader);

// TODO: REMOVE ME ONCE PROTOTYPE IS DONE
    GameCard(
        fivednine::render::ShaderPtr spShader,
        fivednine::render::TexturePtr spTexture);

    GameCard(
        fivednine::render::ShaderPtr spShader,
        fivednine::render::TexturePtr spTexture,
        const glm::vec3& upperLeft,
        uint32_t width,
        uint32_t height);
// TODO: REMOVE ME ONCE PROTOTYPE IS DONE^^^

    void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

    void SetPosition(float x, float y, float z);
    void SetDimensions(float width, float height);

    fivednine::render::TexturePtr GetTexture() const;
    void SetTexture(fivednine::render::TexturePtr spTexture);

private:
    GameCard() = delete;
    GameCard(const GameCard& other) = delete;

    fivednine::render::Mesh       m_cardMesh;

    // All game cards are fundamentally textured unit quads.
    // TODO: instanced rendering for game cards
    static const glm::vec3 s_Vertices[4];
    static const glm::vec2 s_UVs[4];
    static const int       s_Indices[6];
};

typedef std::shared_ptr<GameCard> GameCardPtr;