#pragma once

#include "scene/components/fanComponent.h"

namespace scene
{
	class Transform : public Component
	{
	public:
		Transform(Gameobject * _gameobject);

		void SetPosition(btVector3 _newPosition);
		void SetScale	(btVector3 _newScale);
		void SetRotationEuler( const btVector3 _rotation);
		void SetRotationQuat(  const btQuaternion _rotation);

		btVector3 GetPosition() const		{ return m_position; }
		btVector3 GetScale() const			{ return m_scale; }
		btVector3 GetRotationEuler() const;
		btQuaternion GetRotationQuat() const { return m_rotation; }
		glm::mat4 GetModelMatrix() const;
		btVector3 Right() const;
		btVector3 Forward() const;
		btVector3 Up() const;

		static const btVector3 worldRight;
		static const btVector3 worldUp;
		static const btVector3 worldForward;

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "transform"; }

	private:
		btQuaternion m_rotation;
		btVector3 m_position;
		btVector3 m_scale;
	};
}