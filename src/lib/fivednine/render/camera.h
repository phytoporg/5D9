#pragma once

#include <glm/glm.hpp>

namespace fivednine { namespace render {
    class Camera
    {
    public:
        Camera();
        Camera(
            const glm::vec3& translation,
            const glm::vec3& forwardDirection,
            const glm::vec3& upDirection
        );

        glm::vec3 GetTranslation() const;
        void SetTranslation(const glm::vec3& translation);
        void Translate(const glm::vec3& translationDelta);

        void SetTranslationTarget(const glm::vec3 targetTranslation);

        glm::mat4 ViewMatrix4() const;

        void Tick(float dtSeconds);

    private:
        glm::vec3 m_translation;
        glm::vec3 m_forwardDirection;
        glm::vec3 m_upDirection;

        glm::vec3 m_translationTarget;

        static const glm::vec3 kDefaultTranslation;
        static const glm::vec3 kDefaultForwardDirection;
        static const glm::vec3 kDefaultUpDirection;
    };
}}
