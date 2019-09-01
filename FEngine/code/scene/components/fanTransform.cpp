#include "fanGlobalIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/components/fanComponent.h"
#include "scene/fanGameobject.h"
#include "core/fanSignal.h"

namespace fan
{
	namespace scene
	{
		REGISTER_TYPE_INFO(Transform)

			//================================================================================================================================
			//================================================================================================================================
			void Transform::Initialize() {
			m_rotation = btQuaternion::getIdentity();
			m_position = btVector3(0, 0, 0);
			m_scale = btVector3(1, 1, 1);

			SetRemovable(false);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Transform::SetPosition(btVector3 _newPosition) {
			if (m_position != _newPosition) {
				m_position = _newPosition;

				// 		Rigidbody* rb = GetGameobject()->GetComponent<Rigidbody>();
				// 		if (rb)	{
				// 			rb->SetPosition(newPosition);
				// 		}

				SetModified(true);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Transform::SetScale(btVector3 _newScale)
		{
			if (m_scale != _newScale) {
				m_scale = _newScale;
				SetModified(true);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Transform::SetRotationEuler(const btVector3 _rotation)
		{
			btQuaternion quat;
			quat.setEulerZYX(btRadians(_rotation.z()), btRadians(_rotation.y()), btRadians(_rotation.x()));
			SetRotationQuat(quat);
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
			if (m_rotation != _rotation) {
				m_rotation = _rotation;
				SetModified(true);
			}
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

		//================================================================================================================================
		//================================================================================================================================
		void Transform::Load(std::istream& _in) {
			_in >> m_position[0];		_in >> m_position[1];		_in >> m_position[2];
			_in >> m_rotation[0];		_in >> m_rotation[1];		_in >> m_rotation[2];	_in >> m_rotation[3];
			_in >> m_scale[0];			_in >> m_scale[1];			_in >> m_scale[2];
		}

		//================================================================================================================================
		//================================================================================================================================
		void Transform::Save(std::ostream& _out) {
			_out << "\t\t" << m_position[0] << " " << m_position[1] << " " << m_position[2] << std::endl;
			_out << "\t\t" << m_rotation[0] << " " << m_rotation[1] << " " << m_rotation[2] << " " << m_rotation[3] << std::endl;
			_out << "\t\t" << m_scale[0] << " " << m_scale[1] << " " << m_scale[2] << std::endl;
		}
	}
}