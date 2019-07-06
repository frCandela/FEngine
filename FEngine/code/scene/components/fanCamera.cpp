#include "fanIncludes.h"

#include "scene/components/fanCamera.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetView() const
	{
		Transform* transform = GetGameobject()->GetComponent<Transform>();
		glm::mat4 view = glm::lookAt(transform->GetPosition(), transform->GetPosition() + transform->Forward(), transform->Up());
		return view;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetProjection() const
	{
		glm::mat4 proj = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearDistance, m_farDistance);
		return proj;
	}

	//================================================================================================================================
	//================================================================================================================================
	util::Ray Camera::ScreenPosToRay(glm::vec2 position)
	{
		assert(position.x >= -1.f  && position.x <= 1.f);
		assert(position.y >= -1.f && position.y <= 1.f);

		Transform* transform = GetGameobject()->GetComponent<Transform>();

		const glm::vec3 pos = transform->GetPosition();
		const glm::vec3 upVec = transform->Up();
		const glm::vec3 right = transform->Right();
		const glm::vec3 forward = transform->Forward();

		glm::vec3 nearMiddle = pos + m_nearDistance * forward;

		float nearHeight = m_nearDistance * tan(glm::radians(m_fov / 2));
		float nearWidth = m_aspectRatio * nearHeight;

		util::Ray ray;
		ray.origin = nearMiddle + position.x * nearWidth * right - position.y * nearHeight * upVec;
		ray.direction = 100.f * glm::normalize(ray.origin - pos);

		return ray;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::SetFov(float _fov) {
		m_fov = _fov;
		SetModified(true);
	};
	void Camera::SetNearDistance(float _nearDistance) {
		m_nearDistance = _nearDistance;
		SetModified(true);
	};
	void Camera::SetFarDistance(float _farDistance) {
		m_farDistance = _farDistance;
		SetModified(true);
	};
	void Camera::SetAspectRatio(float _aspectRatio) {
		m_aspectRatio = _aspectRatio;
		SetModified(true);
	};
}