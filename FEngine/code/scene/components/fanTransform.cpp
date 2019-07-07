#include "fanIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "util/fanSignal.h"

namespace scene
{
	const glm::vec3 Transform::worldRight(1.f, 0.f, 0.f);
	const glm::vec3 Transform::worldUp(0.f, 1.f, 0.f);
	const glm::vec3 Transform::worldForward(0.f, 0.f, 1.f);

	//================================================================================================================================
	//================================================================================================================================
	Transform::Transform(Gameobject * _gameobject) : 
		Component(_gameobject)
		, m_rotation	( glm::vec3(0, 0, 0))
		, m_position	( glm::vec3(0, 0, 0))
		, m_scale		( glm::vec3(1, 1, 1)) {
		SetRemovable(false);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetPosition(glm::vec3 _newPosition)
	{
		m_position = _newPosition;

// 		Rigidbody* rb = GetGameobject()->GetComponent<Rigidbody>();
// 		if (rb)	{
// 			rb->SetPosition(newPosition);
// 		}

		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetScale(glm::vec3 _newScale)
	{
		m_scale = _newScale;
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationEuler(const glm::vec3 _rotation)
	{
		m_rotation = _rotation;

		glm::quat xQuat = glm::angleAxis(glm::radians(_rotation.x), worldRight);
		glm::quat yQuat = glm::angleAxis(glm::radians(_rotation.y), worldUp);
		glm::quat zQuat = glm::angleAxis(glm::radians(_rotation.z), worldForward);
		m_rotation = zQuat*yQuat*xQuat;

		// 		Rigidbody* rb = GetGameobject()->GetComponent<Rigidbody>();
		// 		if (rb)	{
		// 			rb->SetRotation(newRotation);
		// 		}
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationQuat(const glm::quat _rotation) {
		m_rotation = _rotation;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::vec3 Transform::GetRotationEuler() const {
		return glm::degrees(eulerAngles(m_rotation));
	}


	glm::mat4 Transform::GetModelMatrix() const { return glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(GetRotationQuat()) * glm::scale(glm::mat4(1.f), m_scale); }
	glm::vec3 Transform::Right() const { return GetRotationQuat() * glm::vec4(1.f, 0.f, 0.f, 1.f); }
	glm::vec3 Transform::Forward() const { return GetRotationQuat() * glm::vec4(0.f, 0.f, 1.f, 1.f); }
	glm::vec3 Transform::Up() const { return GetRotationQuat() * glm::vec4(0.f, 1.f, 0.f, 1.f); }
}