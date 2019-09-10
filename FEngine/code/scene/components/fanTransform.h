#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	namespace scene
	{
		//================================================================================================================================
		//================================================================================================================================
		class Transform : public Component
		{
		public:
			void SetPosition(btVector3 _newPosition);
			void SetScale(btVector3 _newScale);
			void SetRotationEuler(const btVector3 _rotation);
			void SetRotationQuat(const btQuaternion _rotation);

			btTransform		GetBtTransform() const { return btTransform(m_rotation, m_position); }
			btVector3		GetPosition() const { return m_position; }
			btVector3		GetScale() const { return m_scale; }
			btQuaternion	GetRotationQuat() const { return m_rotation; }
			btVector3		GetRotationEuler() const;
			glm::mat4		GetModelMatrix() const;
			glm::mat4		GetRotationMat() const;

			btVector3 Right() const;
			btVector3 Forward() const;
			btVector3 Up() const;

			bool			IsUnique() const		override { return true; }

			// ISerializable
			bool Load(std::istream& _in) override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;

			DECLARE_TYPE_INFO(Transform);
		protected:
			void OnAttach() override;

		private:
			btQuaternion m_rotation;
			btVector3 m_position;
			btVector3 m_scale;
		};
	}
}