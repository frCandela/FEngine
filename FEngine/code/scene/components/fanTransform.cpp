#include "fanIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "util/fanSignal.h"

namespace scene
{
	const btVector3 Transform::worldRight(1.f, 0.f, 0.f);
	const btVector3 Transform::worldUp(0.f, 1.f, 0.f);
	const btVector3 Transform::worldForward(0.f, 0.f, 1.f);

	//================================================================================================================================
	//================================================================================================================================
	Transform::Transform(Gameobject * _gameobject) : 
		Component(_gameobject)
		, m_rotation( btQuaternion::getIdentity())
		, m_position( btVector3(0, 0, 0))
		, m_scale	( btVector3(1, 1, 1)) {
		SetRemovable(false);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetPosition(btVector3 _newPosition)
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
	void Transform::SetScale(btVector3 _newScale)
	{
		m_scale = _newScale;
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationEuler(const btVector3 _rotation)
	{
		m_rotation.setEuler( btRadians(_rotation.x()), btRadians(_rotation.y()), btRadians(_rotation.z()) );

// 		glm::quat xQuat = glm::angleAxis(glm::radians(_rotation.x), worldRight);
// 		glm::quat yQuat = glm::angleAxis(glm::radians(_rotation.y), worldUp);
// 		glm::quat zQuat = glm::angleAxis(glm::radians(_rotation.z), worldForward);
// 		m_rotation = zQuat*yQuat*xQuat;

		// 		Rigidbody* rb = GetGameobject()->GetComponent<Rigidbody>();
		// 		if (rb)	{
		// 			rb->SetRotation(newRotation);
		// 		}
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationQuat(const btQuaternion _rotation) {
		m_rotation = _rotation;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::GetRotationEuler() const {
		btVector3 euler;
		m_rotation.getEulerZYX(euler[0], euler[1], euler[2]);
		return btDegrees3(euler);
	}


	glm::mat4 Transform::GetModelMatrix() const { 

		glm::vec3 position(m_position[0], m_position[1], m_position[2]);
		glm::vec3 scale(m_scale[0], m_scale[1], m_scale[2]);
		glm::quat rotation(m_rotation.getX(), m_rotation.getY(), m_rotation.getZ(), m_rotation.getW());

		return glm::translate(glm::mat4(1.f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.f), scale);
	}
	btVector3 Transform::Right() const {
		btTransform t(m_rotation, btVector3(0, 0, 0));
		return t * worldRight;	
	}
	btVector3 Transform::Forward() const {
		btTransform t(m_rotation, btVector3(0, 0, 0));
		return t * worldForward;
	}
	btVector3 Transform::Up() const {
		btTransform t(m_rotation, btVector3(0, 0, 0));
		return t * worldUp;
	}
}