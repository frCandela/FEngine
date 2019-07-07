#pragma once

#include "scene/components/fanComponent.h"

namespace scene
{
	class Transform : public Component
	{
	public:
		Transform(Gameobject * _gameobject);

		void SetPosition( glm::vec3 _newPosition);
		void SetScale	( glm::vec3 _newScale);
		void SetRotationEuler( const glm::vec3 _rotation);
		void SetRotationQuat(  const glm::quat _rotation);

		glm::vec3 GetPosition() const		{ return m_position; }
		glm::vec3 GetScale() const			{ return m_scale; }
		glm::vec3 GetRotationEuler() const;
		glm::quat GetRotationQuat() const { return m_rotation; }
		glm::mat4 GetModelMatrix() const;
		glm::vec3 Right() const; 
		glm::vec3 Forward() const;
		glm::vec3 Up() const;

		static const glm::vec3 worldRight;
		static const glm::vec3 worldUp;
		static const glm::vec3 worldForward;

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "transform"; }

	private:
		glm::quat m_rotation;
		glm::vec3 m_position;
		glm::vec3 m_scale;
	};
}