#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace fivednine::render;

const glm::vec3 Camera::kDefaultTranslation = glm::vec3(0.f);
const glm::vec3 Camera::kDefaultForwardDirection = glm::vec3(0.f, 0.f, -1.f);
const glm::vec3 Camera::kDefaultUpDirection = glm::vec3(0.f, 1.f, 0.f);

Camera::Camera()
    : Camera(kDefaultTranslation, kDefaultForwardDirection, kDefaultUpDirection)
{}

Camera::Camera(
    const glm::vec3& translation,
    const glm::vec3& forwardDirection,
    const glm::vec3& upDirection
    ) : m_translation(translation),
        m_forwardDirection(forwardDirection),
        m_upDirection(upDirection)
{}

glm::vec3 Camera::GetTranslation() const 
{
    return m_translation;
}

void Camera::SetTranslation(const glm::vec3& translation)
{
    m_translation = translation;
}

void Camera::Translate(const glm::vec3& translationDelta) 
{
    m_translation += translationDelta;
}

glm::mat4 Camera::ViewMatrix4() const 
{
    return glm::lookAt(
        m_translation,
        (m_translation + m_forwardDirection),
        m_upDirection);
}
