#pragma once

#include "scene/components/fanComponent.h"

namespace scene
{
	class Transform : public Component
	{
	public:
		Transform();

		void SetPosition( glm::vec3 newPosition);
		void SetScale	( glm::vec3 newScale);
		void SetRotation( glm::quat newRotation);

		glm::vec3 GetPosition() const		{ return m_position; }
		glm::vec3 GetScale() const			{ return m_scale; }
		glm::quat GetRotation() const		{ return m_rotation; }
		glm::mat4 GetModelMatrix() const	{ return	glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.f), m_scale); }
		glm::vec3 Right() const				{ return	m_rotation * glm::vec3(1.f, 0, 0); }
		glm::vec3 Forward() const			{ return	m_rotation * glm::vec3(0, 0, 1); }
		glm::vec3 Up() const				{ return	m_rotation * glm::vec3(0, 1, 0); }

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "Transform"; }

	private:
		glm::quat m_rotation;
		glm::vec3 m_position;
		glm::vec3 m_scale;
	};
}