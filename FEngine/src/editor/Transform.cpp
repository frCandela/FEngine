#include "editor/Transform.h"

void Transform::RenderGui()
{
	ImGui::Text("Transform");

	if (ImGui::Button("##TransPos")) 
		position = glm::vec3(0, 0, 0);
	ImGui::SameLine();
	ImGui::DragFloat3("position", reinterpret_cast<float*>(&position), 0.1f);	

	if (ImGui::Button("##TransRot")) 
		rotation = glm::quat(0, 0, 0, 1);
	ImGui::SameLine();
	glm::vec3 euler = glm::eulerAngles(rotation);
	float angles[3] = { glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z) };
	ImGui::DragFloat3("rotation", angles, 0.1f);
	rotation = glm::quat(glm::vec3(glm::radians(angles[0]), glm::radians(angles[1]), glm::radians(angles[2])));		

	if (ImGui::Button("##TransScale"))
		scale = glm::vec3(1, 1, 1);
	ImGui::SameLine();
	ImGui::DragFloat3("scale", reinterpret_cast<float*>(&scale), 0.1f);
}

glm::mat4 Transform::GetModelMatrix() const{return  glm::translate( glm::mat4(1.f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.f), scale);}
glm::vec3 Transform::Right() const { return  rotation * glm::vec3(1.f, 0, 0); } 
glm::vec3 Transform::Forward() const { return  rotation * glm::vec3(0, 0, 1); }
glm::vec3 Transform::Up() const { return rotation * glm::vec3(0, -1, 0); }//??