#include "fanIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "util/fanSignal.h"

namespace scene
{
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
		m_rotation.setEulerZYX( btRadians(_rotation.z()), btRadians(_rotation.y()), btRadians(_rotation.x()) );
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::GetRotationEuler() const {
		btVector3 euler;
		m_rotation.getEulerZYX(euler[2], euler[1], euler[0]);
		return btDegrees3(euler);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationQuat(const btQuaternion _rotation) {
		m_rotation = _rotation;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Transform::GetModelMatrix() const { 

		glm::vec3 position(m_position[0], m_position[1], m_position[2]);
		glm::vec3 scale(m_scale[0], m_scale[1], m_scale[2]);
		glm::quat rotation(m_rotation.getX(), m_rotation.getY(), m_rotation.getZ(), m_rotation.getW());

		return glm::translate(glm::mat4(1.f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.f), scale);
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Transform::GetRotationMat() const {
		glm::quat rotation(m_rotation.getX(), m_rotation.getY(), m_rotation.getZ(), m_rotation.getW());
		return glm::mat4_cast(rotation);
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::Right() const {
		btTransform t(m_rotation, btVector3(0, 0, 0));
		return t * btVector3::Right();
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::Forward() const {
		btTransform t(m_rotation, btVector3(0, 0, 0));
		return t * btVector3::Forward();
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::Up() const {
		btTransform t(m_rotation, btVector3(0, 0, 0));
		return t * btVector3::Up();
	}
}