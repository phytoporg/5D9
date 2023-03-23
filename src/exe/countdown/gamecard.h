#pragma once

#include <glm/glm.hpp>
#include <countdown/render/mesh.h>
#include <countdown/render/shader.h>
#include <countdown/render/texture.h>

class GameCard
{
public:
    GameCard(
        countdown::render::ShaderPtr spShader,
        countdown::render::TexturePtr spTexture,
        const glm::vec3& upperLeft,
        uint32_t width,
        uint32_t height);

    void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

    countdown::render::TexturePtr GetTexture() const;

private:
    GameCard() = delete;
    GameCard(const GameCard& other) = delete;

    countdown::render::Mesh       m_cardMesh;
    countdown::render::TexturePtr m_spTexture;

    // All game cards are fundamentally textured unit quads.
    // TODO: instanced rendering for game cards
    static const glm::vec3 s_Vertices[4];
    static const glm::vec2 s_UVs[4];
    static const int       s_Indices[6];
};