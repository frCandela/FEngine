#include "fanIncludes.h"

#include "scene/components/fanCamera.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace scene
{	
	const char * Camera::s_name = "camera";
	const uint32_t Camera::s_type = Component::Register<Camera>(SSID("camera"));

	//================================================================================================================================
	//================================================================================================================================
	Camera::Camera() {

	}

	//================================================================================================================================
	//================================================================================================================================
	Camera::Camera(Gameobject * _gameobject) : 
		Component(_gameobject) 
	{
		m_fov			= 110.f;
		m_aspectRatio	= 1.f;
		m_nearDistance	= 0.01f;
		m_farDistance	= 1000.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetView() const
	{
		Transform* transform = GetGameobject()->GetComponent<Transform>();
		glm::mat4 view = glm::lookAt(
			util::ToGLM(transform->GetPosition()), 
			util::ToGLM(transform->GetPosition() + transform->Forward()),
			util::ToGLM(transform->Up()));
		return view;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetProjection() const
	{
		const float fov = glm::radians(m_fov);
		glm::mat4 proj = glm::perspective(fov, m_aspectRatio, m_nearDistance, m_farDistance);
		return proj;
	}

	//================================================================================================================================
	//================================================================================================================================
	shape::Ray Camera::ScreenPosToRay(const btVector2& _position)
	{
		assert(_position.x() >= -1.f  && _position.x() <= 1.f);
		assert(_position.y() >= -1.f && _position.y() <= 1.f);

		Transform* transform = GetGameobject()->GetComponent<Transform>();

		const btVector3	 pos = transform->GetPosition();
		const btVector3 upVec = transform->Up();
		const btVector3 right = transform->Right();
		const btVector3 forward = transform->Forward();

		btVector3 nearMiddle = pos + m_nearDistance * forward;

		float nearHeight = m_nearDistance * tan(glm::radians(m_fov / 2));
		float nearWidth = m_aspectRatio * nearHeight;

		shape::Ray ray;
		ray.origin = nearMiddle + _position.x() * nearWidth * right - _position.y() * nearHeight * upVec;
		ray.direction = (100.f * (ray.origin - pos)).normalized();

		return ray;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector2 Camera::WorldPosToScreen(const btVector3& worldPosition) {
		const glm::vec4 pos(worldPosition[0], worldPosition[1], worldPosition[2], 1.f);
		glm::vec4  proj = GetProjection() * GetView() * pos;
		proj /= proj.z;
		return btVector2(proj.x, proj.y);
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

	//================================================================================================================================
	//================================================================================================================================
	void Camera::Load(std::istream& _in) {
		_in >>  m_fov;
		_in >> m_nearDistance;
		_in >> m_farDistance;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::Save(std::ostream& _out) {
		_out << "\t\t" << m_fov << std::endl;
		_out << "\t\t" << m_nearDistance << std::endl;
		_out << "\t\t" << m_farDistance << std::endl;
	}
}