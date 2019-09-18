#include "fanGlobalIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/components/fanComponent.h"
#include "scene/fanEntity.h"
#include "core/fanSignal.h"

namespace fan
{
	namespace scene
	{
		REGISTER_TYPE_INFO(Transform)

		//================================================================================================================================
		//================================================================================================================================
		void Transform::OnAttach() {
			Component::OnAttach();
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

				// 		Rigidbody* rb = Getentity()->GetComponent<Rigidbody>();
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
				SetModified( true ); 
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
			glm::quat rotation;
			rotation.x = m_rotation[0];
			rotation.y = m_rotation[1];
			rotation.z = m_rotation[2];
			rotation.w = m_rotation[3]; 			

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
		btVector3 Transform::Left() const {
			btTransform t(m_rotation, btVector3(0, 0, 0));
			return t * btVector3::Left();
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
		void Transform::OnGui() {
			Component::OnGui(); 

			// Position
			if (ImGui::Button("##TransPos")) {
				SetPosition(btVector3(0, 0, 0));
			} ImGui::SameLine();
			float posBuffer[3] = { GetPosition().x(), GetPosition().y(), GetPosition().z() };
			if (ImGui::DragFloat3("position", posBuffer, 0.1f)) {
				SetPosition(btVector3(posBuffer[0], posBuffer[1], posBuffer[2]));
			}

			// rotation
			if (ImGui::Button("##TransRot")) {
				SetRotationQuat( btQuaternion::getIdentity());
			} ImGui::SameLine();
			const btVector3 rot = GetRotationEuler();
			float bufferAngles[3] = { rot.x(),rot.y(),rot.z() };
			if (ImGui::DragFloat3("rotation", bufferAngles, 0.1f)) {
				SetRotationEuler(btVector3(bufferAngles[0], bufferAngles[1], bufferAngles[2]));
			}

			// Scale
			if (ImGui::Button("##TransScale")) {
				SetScale(btVector3(1, 1, 1));
			} ImGui::SameLine();
			btVector3 scale = GetScale();
			if (ImGui::DragFloat3("scale", &scale[0], 0.1f)) {
				SetScale(scale);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Transform::Load(std::istream& _in) {
			if (!ReadSegmentHeader(_in, "position:")) { return false; }
			if (!ReadFloat(_in, m_position[0])) { return false; }
			if (!ReadFloat(_in, m_position[1])) { return false; }
			if (!ReadFloat(_in, m_position[2])) { return false; }

			if (!ReadSegmentHeader(_in, "rotation:")) { return false; }
			if (!ReadFloat(_in, m_rotation[0])) { return false; }
			if (!ReadFloat(_in, m_rotation[1])) { return false; }
			if (!ReadFloat(_in, m_rotation[2])) { return false; }
			if (!ReadFloat(_in, m_rotation[3])) { return false; }

			if (!ReadSegmentHeader(_in, "scale:")) { return false; }
			if (!ReadFloat(_in, m_scale[0])) { return false; }
			if (!ReadFloat(_in, m_scale[1])) { return false; }
			if (!ReadFloat(_in, m_scale[2])) { return false; }
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Transform::Save(std::ostream& _out, const int _indentLevel) const {
			const std::string indentation = GetIndentation(_indentLevel);
			_out << indentation << "position: " << m_position[0] << " " << m_position[1] << " " << m_position[2] << std::endl;
			_out << indentation << "rotation: " << m_rotation[0] << " " << m_rotation[1] << " " << m_rotation[2] << " " << m_rotation[3] << std::endl;
			_out << indentation << "scale:    " << m_scale[0] << " " << m_scale[1] << " " << m_scale[2] << std::endl;
			return true;
		}
	}
}