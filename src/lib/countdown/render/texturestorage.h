#pragma once

#include "texture.h"

#include <string>
#include <cstdint>
#include <vector>

namespace countdown { namespace render {
    struct ImageData 
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t Depth;
        uint8_t* pBytes;
    };

    class TextureStorage 
    {
    public:
        bool 
        AddTextureFromImagePath(
            const std::string& imagePath,
            const std::string& textureName
            );

        bool 
        AddTexture(
            const ImageData& imageData,
            const std::string& textureName
            );

        TexturePtr FindTextureByName(const std::string& textureName) const;

    private:
        virtual bool AddResource(Texture* pTexture);

        // Using a vector for now because I don't anticipate using many 
        // textures in the foreseeable future; linear operations are
        // probably preferable, and we can change that if this things grow in
        // scope and this assumption ever becomes untrue.
        std::vector<TexturePtr> m_storageVector;
    };
}}
