#include "editor/Scene.h"
#include "editor/Transform.h"

#include "editor/Mesh.h"

#include "glm/gtx/norm.hpp"

Scene::Scene(std::string name) :
	m_name(name)
{}

GameObject* Scene::CreateGameobject(std::string name)
{
	GameObject* gameObject = new GameObject(name);
	m_gameObjects[gameObject] = gameObject;
	gameObject->AddComponent<Transform>();
	return gameObject;
}

void Scene::DeleteGameobjectLater(GameObject* gameobject)
{
	m_toDeleteLater.push_back(gameobject);
}

std::vector<GameObject * > Scene::GetGameobjectsToDelete() const
{
	return m_toDeleteLater;
}

void Scene::ApplyDeleteCommands()
{
	// Deletes every GameObject in the m_toDeleteLater vector
	for (GameObject* gameobject : m_toDeleteLater)
	{
		m_gameObjects.erase(gameobject);
		delete(gameobject);
		m_gameObjectsAABB.erase(gameobject);
	}
	m_toDeleteLater.clear();
}



void Scene::RenderInspectorGui()
{
	ImGui::Begin("Inspector");
	if (m_gameObjectSelected)
	{
		// Gameobject gui
		m_gameObjectSelected->RenderGui();

		float width = ImGui::GetWindowSize().x / 2.f;


		//Delete button
		if (ImGui::Button("Delete"))
		{
			DeleteGameobjectLater(m_gameObjectSelected);
			m_gameObjectSelected = nullptr;
		}	
	}
	ImGui::End();
}

void Scene::RenderSceneGui()
{
	ImGui::Begin("Scene hierarchy");
	ImGui::Text(m_name.c_str());
	ImGui::Separator();
	for (std::pair<GameObject*, GameObject*> pair : m_gameObjects )
	{
		GameObject* gameObject = pair.first;

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (gameObject == m_gameObjectSelected ? ImGuiTreeNodeFlags_Selected : 0);
		bool nodeOpen = ImGui::TreeNodeEx(gameObject->GetName().c_str(), node_flags);
		if (ImGui::IsItemClicked())
			m_gameObjectSelected = gameObject;
		if (nodeOpen)		
			ImGui::TreePop();		
	}
	ImGui::End();
}

GameObject * Scene::RayCast(Ray ray )
{
	GameObject * closestTarget = nullptr;
	float closestDistance = std::numeric_limits<float>::max();

	// Iterates through every gameobjects AABB to find the closest one
	for (std::pair<GameObject* const, std::pair<Cube, float > >& pair : m_gameObjectsAABB)
	{
		for (const Triangle& triangle : pair.second.first.GetTriangles())
		{
			glm::vec3 intersection;
			if (triangle.RayCast(ray.origin, ray.direction, &intersection))
			{
				float distance = glm::distance2(intersection, ray.origin);
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestTarget = pair.first;
				}
			}
		}
	}

	return closestTarget;

}

void Scene::DrawSelectionAABB(Renderer& renderer)
{
	// Shows the AABB of the selected gameobject
	if (GetSelectedGameobject())
	{
		auto it = m_gameObjectsAABB.find(GetSelectedGameobject());

		// If the AABB exists
		if (it != m_gameObjectsAABB.end())
		{
			Cube& cube = it->second.first;
			for (const Triangle& triangle : cube.GetTriangles())
			{
				renderer.DebugLine(triangle.v0, triangle.v1);
				renderer.DebugLine(triangle.v1, triangle.v2);
				renderer.DebugLine(triangle.v2, triangle.v0);
			}
		}
	}
}

void Scene::UpdateGameobjectAABB(GameObject * gameobject, bool updateGeometry)
{
	Mesh* mesh = gameobject->GetComponent<Mesh>();
	if (mesh)
	{
		if (updateGeometry)
		{
			// Recreate the AABB using to the mesh geometry

			if (mesh)
			{
				float maxX = 0.f;
				float maxY = 0.f;
				float maxZ = 0.f;

				// Iterates throught every vertex to find the boundaries
				for (ForwardPipeline::Vertex vertex : mesh->vertices)
				{
					float x = std::abs(vertex.pos.x);
					if (x > maxX)
						maxX = x;
					float y = std::abs(vertex.pos.y);
					if (y > maxY)
						maxY = y;
					float z = std::abs(vertex.pos.z);
					if (z > maxZ)
						maxZ = z;
				}

				Transform* transform = gameobject->GetComponent<Transform>();
				float maxScale = std::max(std::abs(transform->GetScale().x), std::max(std::abs(transform->GetScale().y), std::abs(transform->GetScale().z)));
				float halfSize = std::max(maxX, std::max(maxY, maxZ));

				m_gameObjectsAABB[gameobject] = std::make_pair(Cube(transform->GetPosition(), maxScale * halfSize), halfSize);
			}
		}
		else
		{
			// Update the AABB position and scale
			Transform* transform = gameobject->GetComponent<Transform>();
			float maxScale = std::max(std::abs(transform->GetScale().x), std::max(std::abs(transform->GetScale().y), std::abs(transform->GetScale().z)));
			float halfSize = m_gameObjectsAABB[gameobject].second;

			m_gameObjectsAABB[gameobject].first = Cube(transform->GetPosition(), maxScale * halfSize);
		}
	}
}

GameObject* Scene::GetSelectedGameobject() { return m_gameObjectSelected; }
void Scene::SetSelectedGameobject(GameObject * newSelectedGameobject) { m_gameObjectSelected = newSelectedGameobject; }