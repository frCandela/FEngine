#pragma once

#include "scene/components/fanComponent.h"

namespace scene
{
	class Transform : public Component
	{
	public:
		Transform(Gameobject * _gameobject);

		void SetPosition( glm::vec3 newPosition);
		void SetScale	( glm::vec3 newScale);
		void SetRotation( glm::vec3 newRotation);

		glm::vec3 GetPosition() const		{ return m_position; }
		glm::vec3 GetScale() const			{ return m_scale; }
		glm::vec3 GetRotation() const		{ return m_rotation; }
		glm::quat GetRotationQuat() const	{ return glm::quat(glm::vec3(glm::radians(m_rotation.x), glm::radians(m_rotation.y), glm::radians(m_rotation.z))); }
		glm::mat4 GetModelMatrix() const	{ return glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(GetRotationQuat()) * glm::scale(glm::mat4(1.f), m_scale); }
		glm::vec3 Right() const				{ return GetRotationQuat() * glm::vec4(1.f, 0.f, 0.f, 1.f); }
		glm::vec3 Forward() const			{ return GetRotationQuat() * glm::vec4(0.f, 0.f, 1.f, 1.f); }
		glm::vec3 Up() const				{ return GetRotationQuat() * glm::vec4(0.f, 1.f, 0.f, 1.f); }

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "transform"; }

	private:
		glm::vec3 m_rotation;
		glm::vec3 m_position;
		glm::vec3 m_scale;
	};
}