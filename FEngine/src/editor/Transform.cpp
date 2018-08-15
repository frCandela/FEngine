#include "editor/Transform.h"

void Transform::RenderGui()
{
	Component::RenderGui();

	// Position
	if (ImGui::Button("##TransPos")) 
		SetPosition(glm::vec3(0, 0, 0));
	ImGui::SameLine();
	float posBuffer[3] = { m_position.x, m_position.y, m_position.z };
	if (ImGui::DragFloat3("position", posBuffer, 0.1f))
		SetPosition(glm::vec3(posBuffer[0], posBuffer[1], posBuffer[2]));

	// rotation
	if (ImGui::Button("##TransRot")) 
		SetRotation(glm::quat(0, 0, 0, 1));
	ImGui::SameLine();
	glm::vec3 euler = glm::eulerAngles(m_rotation);
	float angles[3] = { glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z) };
	if(ImGui::DragFloat3("rotation", angles, 0.1f))
		SetRotation(glm::quat(glm::vec3(glm::radians(angles[0]), glm::radians(angles[1]), glm::radians(angles[2]))));		

	// Scale
	if (ImGui::Button("##TransScale"))
		SetScale( glm::vec3(1, 1, 1));
	ImGui::SameLine();
	float scaleBuffer[3] = { m_scale.x, m_scale.y, m_scale.z };
	if (ImGui::DragFloat3("scale", scaleBuffer, 0.1f))
		SetScale(glm::vec3(scaleBuffer[0], scaleBuffer[1], scaleBuffer[2]));
}

void Transform::SetPosition(glm::vec3 newPosition) {
	m_position = newPosition; m_wasModified = true;
}
void Transform::SetScale(glm::vec3 newScale) { m_scale = newScale; m_wasModified = true; }
void Transform::SetRotation(glm::quat newRotation) { m_rotation = newRotation; m_wasModified = true; }

glm::vec3 Transform::GetPosition() const { return m_position; }
glm::vec3 Transform::GetScale() const { return m_scale; }
glm::quat Transform::GetRotation() const { return m_rotation; }

glm::mat4 Transform::GetModelMatrix() const{return  glm::translate( glm::mat4(1.f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.f), m_scale);}
glm::vec3 Transform::Right() const { return  m_rotation * glm::vec3(1.f, 0, 0); } 
glm::vec3 Transform::Forward() const { return  m_rotation * glm::vec3(0, 0, 1); }
glm::vec3 Transform::Up() const { return m_rotation * glm::vec3(0, -1, 0); }//??