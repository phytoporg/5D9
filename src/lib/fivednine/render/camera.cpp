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
{
    m_translationTarget = m_translation;
}

glm::vec3 Camera::GetTranslation() const 
{
    return m_translation;
}

void Camera::SetTranslation(const glm::vec3& translation)
{
    m_translation = translation;
    m_translationTarget = m_translation;
}

void Camera::Translate(const glm::vec3& translationDelta) 
{
    m_translation += translationDelta;
}

void Camera::SetTranslationTarget(const glm::vec3 translationTarget)
{
    m_translationTarget = translationTarget;
}

glm::mat4 Camera::ViewMatrix4() const 
{
    return glm::lookAt(
        m_translation,
        (m_translation + m_forwardDirection),
        m_upDirection);
}

void Camera::Tick(float dtSeconds)
{
    // TODO: Proper dampened spring model?
    // TODO: externally-configurable constants
    const float kTranslationSpeed = 0.01f;
    const glm::vec3 Delta = m_translationTarget - m_translation;

    const float kMinDistance = 0.001f;
    if (Delta.length() > kMinDistance)
    {
        m_translation += kTranslationSpeed * Delta * dtSeconds;
    }
}