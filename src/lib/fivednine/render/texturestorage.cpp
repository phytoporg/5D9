#include "texturestorage.h"
#include "rendercommon.h"

#include <SDL.h>
#include <SDL_image.h>

#include <algorithm>

#include <fivednine/log/log.h>

// Probably belongs in a more visible spot, but this isn't yet needed anywhere else.
namespace {
    static bool IsIntegerPowerOfTwo(int integer)
    {
        // Any power-of-two integer has a single set bit in its binary representation, so ANDing
        // with its decrement will always produce zero. This test only works if the integer 
        // being tested is greater than one to begin with. Since 2^0 == 1, we'll consider that
        // case separately.

        // i.e.
        // Power of two: 1000 & 0111 = 0000
        // Not a power of two: 1010 & 1001 = 1000 <-- More than one bit

        return integer == 1 || (integer > 1 && ((integer - 1) & integer) == 0);
    }
}

using namespace fivednine;
using namespace fivednine::render;

bool 
TextureStorage::AddTextureFromImagePath(
    const std::string& imagePath,
    const std::string& textureName
    )
{
    SDL_Surface* pSurface = IMG_Load(imagePath.c_str());
    if (!pSurface)
    {
        RELEASE_LOG_WARNING(LOG_RENDER, "Failed to load image from path: %s", imagePath.c_str());
        return false;
    }

    ImageData imageData {
        static_cast<uint32_t>(pSurface->w),
        static_cast<uint32_t>(pSurface->h),
        pSurface->format->BytesPerPixel,
        static_cast<uint8_t*>(pSurface->pixels)
    };

    return AddTexture(imageData, textureName);
}

bool 
TextureStorage::AddTexture(
    const ImageData& imageData,
    const std::string& textureName
    )
{
    // Check to see if dimensions are a power of two.
    if (!IsIntegerPowerOfTwo(imageData.Width) || !IsIntegerPowerOfTwo(imageData.Height))
    {
        // Not catastrophic, but may result in unexpected behaviors.
        RELEASE_LOGLINE_WARNING(
            LOG_RENDER,
            "Texture named '%s' has dimensions which are not a power of two.",
            textureName.c_str());
    }

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Scale linearly when image is larger than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    // Scale linearly when image is smaller than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

    glPixelStorei(GL_PACK_ALIGNMENT, 1); 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

    // TODO: DOn't assume that 4BPP => RGBA & RGB otherwise
    glTexImage2D(
        GL_TEXTURE_2D, 0, imageData.Depth == 4 ? GL_RGBA : GL_RGB,
        imageData.Width, imageData.Height, 
        0, 
        imageData.Depth == 4 ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        imageData.pBytes);

    Texture* pTexture = 
        new Texture(
            textureName,
                textureId,
                imageData.Width,
                imageData.Height,
                imageData.Depth);
    if (!AddResource(pTexture))
    {
        delete pTexture;
        return false;
    }

    return true;
}

TexturePtr 
TextureStorage::FindTextureByName(const std::string& textureName) const
{
    for (const TexturePtr& spTexture : m_storageVector)
    {
        if (spTexture->GetName() == textureName)
        {
            return spTexture;
        }
    }

    return nullptr;
}

bool TextureStorage::AddResource(Texture* pTexture)
{
    if (!pTexture) { return false; }

    if (!FindTextureByName(pTexture->GetName()))
    {
        m_storageVector.emplace_back(pTexture);
        RELEASE_LOGLINE_INFO(LOG_RENDER, "Added texture to storage: %s", pTexture->GetName().c_str());
        return true;
    }
    else
    {
        RELEASE_LOGLINE_WARNING(LOG_RENDER, "Failed to add texture %s to storage: already exists.", pTexture->GetName().c_str());
        return false;
    }
}
