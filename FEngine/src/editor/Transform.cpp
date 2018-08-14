#include "editor/Transform.h"

void Transform::RenderGui()
{
	ImGui::Text("Transform");

	if (ImGui::Button("##TransPos")) 
		m_position = glm::vec3(0, 0, 0);
	ImGui::SameLine();
	ImGui::DragFloat3("position", reinterpret_cast<float*>(&m_position), 0.1f);	

	if (ImGui::Button("##TransRot")) 
		m_rotation = glm::quat(0, 0, 0, 1);
	ImGui::SameLine();
	glm::vec3 euler = glm::eulerAngles(m_rotation);
	float angles[3] = { glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z) };
	ImGui::DragFloat3("rotation", angles, 0.1f);
	m_rotation = glm::quat(glm::vec3(glm::radians(angles[0]), glm::radians(angles[1]), glm::radians(angles[2])));		

	if (ImGui::Button("##TransScale"))
		m_scale = glm::vec3(1, 1, 1);
	ImGui::SameLine();
	ImGui::DragFloat3("scale", reinterpret_cast<float*>(&m_scale), 0.1f);
}

void Transform::SetPosition(glm::vec3 newPosition) { m_position = newPosition; }
void Transform::SetScale(glm::vec3 newScale) { m_scale = newScale; }
void Transform::SetRotation(glm::quat newRotation) { m_rotation = newRotation; }

glm::vec3 Transform::GetPosition() const { return m_position; }
glm::vec3 Transform::GetScale() const { return m_scale; }
glm::quat Transform::GetRotation() const { return m_rotation; }

glm::mat4 Transform::GetModelMatrix() const{return  glm::translate( glm::mat4(1.f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.f), m_scale);}
glm::vec3 Transform::Right() const { return  m_rotation * glm::vec3(1.f, 0, 0); } 
glm::vec3 Transform::Forward() const { return  m_rotation * glm::vec3(0, 0, 1); }
glm::vec3 Transform::Up() const { return m_rotation * glm::vec3(0, -1, 0); }//??