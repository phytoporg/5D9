#pragma once

#include "shader.h"
#include "texture.h"
#include "uniform.h"
#include <cstring>
#include <vector>
#include <initializer_list>
#include <fivednine/render/attribute.h>
#include <fivednine/render/indexbuffer.h>

#include <glm/glm.hpp>

namespace fivednine { namespace render {

    struct MeshUniformValue
    {
        MeshUniformValue() {}

        MeshUniformValue(const char* pUniformName, UniformType type, void const* pValue)
        : Type(type), pValue(pValue) 
        {
            strncpy(Name, pUniformName, sizeof(Name));
        }

        MeshUniformValue(const MeshUniformValue& other)
        {
            memcpy(this, &other, sizeof(MeshUniformValue));
        }

        MeshUniformValue operator=(const MeshUniformValue& other)
        {
            memcpy(this, &other, sizeof(MeshUniformValue));
            return *this;
        }

        char Name[256] = {};
        UniformType Type = UniformType::Invalid;
        void const* pValue = nullptr;

        // TO cache the name -> slot lookups
        uint32_t Location = Shader::kInvalidHandleValue;
    };

    class Mesh 
    {
        public:
            Mesh(size_t maxVertexCount, size_t maxIndexCount);
            Mesh(const Mesh& other);
            Mesh& operator=(const Mesh& other);

            bool SetPositions(const glm::vec3* positionArray, uint32_t arrayLen);
            bool SetTextureCoordinates(const glm::vec2* uvArray, uint32_t arrayLen);
            bool SetIndices(const int* indicesArray, uint32_t arrayLen);

            bool SetShader(ShaderPtr spShader);
            void SetModelMatrix(const glm::mat4& modelMatrix);
            glm::mat4& GetModelMatrix();
            const glm::mat4& GetModelMatrix() const;

            void SetTexture(TexturePtr spTexture);
            TexturePtr GetTexture() const;

            // To be set for each draw call
            void SetMeshUniforms(const std::initializer_list<MeshUniformValue>& uniformValues);
            void SetMeshUniforms(const std::vector<MeshUniformValue>& uniformValues);

            void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

            void SetVisibility(bool newVisibility);
            bool GetVisibility() const;

            // Force a render state update
            void MarkDirty();

        private:
            void CopyTo(Mesh& other) const;
            void BindAttributes();
            void UnbindAttributes();

            // Attributes
            Attribute<glm::vec3>   m_VertexPositionAttribute;
            Attribute<glm::vec2>   m_VertexTextureCoordinateAttribute;
            IndexBuffer            m_Indices;

            // Data
            std::vector<glm::vec3> m_VertexPositions;
            std::vector<glm::vec2> m_VertexTextureCoordinates;
            std::vector<uint32_t>  m_VertexIndices;

            static const size_t kMaxMeshUniforms = 8;
            std::vector<MeshUniformValue> m_MeshUniformValues;

            glm::mat4              m_ModelMatrix;

            ShaderPtr              m_spShader;

            uint32_t               m_VAO;

            uint32_t               m_ModelUniformHandle;
            uint32_t               m_ViewUniformHandle;
            uint32_t               m_ProjUniformHandle;
            uint32_t               m_SamplerUniformHandle;

            TexturePtr             m_spTexture;

            bool                   m_isDirty   = true;
            bool                   m_isVisible = true;
    };
}}
