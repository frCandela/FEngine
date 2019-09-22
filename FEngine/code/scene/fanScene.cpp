#include "fanGlobalIncludes.h"

#include "scene/fanScene.h"
#include "scene/fanEntity.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanActor.h"
#include "core/fanSignal.h"
#include "fanEngine.h"

namespace fan
{
	Signal<>		Scene::s_onSceneClear;
	Signal<Scene*>	Scene::s_onSceneLoad;

	//================================================================================================================================
	//================================================================================================================================
	Scene::Scene(const std::string _name) :
		m_name(_name)
		, m_path("")
		, m_root(nullptr) {

		Actor::onActorAttach.Connect(&Scene::OnActorAttach, this);
		Actor::onActorDetach.Connect(&Scene::OnActorDetach, this);
	}

	//================================================================================================================================
	//================================================================================================================================
	Scene::~Scene() {
		Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	Entity *	Scene::CreateEntity(const std::string _name, Entity * _parent) {
		if (_parent == nullptr) {
			_parent = m_root;
		}
		Entity* entity = new Entity(_name, _parent);
		entity->SetScene(this);

		return entity;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::DeleteEntity(Entity* _entity) {
		m_entitiesToDelete.push_back(_entity);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::R_BuildEntitiesList(Entity* _entity, std::vector<Entity*>& _entitiesList) const {
		_entitiesList.push_back(_entity);
		const std::vector<Entity*>& childs = _entity->GetChilds();
		for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
			R_BuildEntitiesList(childs[childIndex], _entitiesList);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Component *	Scene::R_FindComponentOfType(Entity * _entity, const uint32_t _typeID) const {

		// Search in components
		const std::vector<Component*> & components = _entity->GetComponents();
		for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
			Component* component = components[componentIndex];
			if (component->GetType() == _typeID) {
				return component;
			}
		}

		// Recursive call to child entities
		const std::vector<Entity*> & childEntities = _entity->GetChilds();
		for (int childIndex = 0; childIndex < childEntities.size(); childIndex++) {
			Component* component = R_FindComponentOfType(childEntities[childIndex], _typeID);
			if (component != nullptr) {
				return component;
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < Entity * >  Scene::BuildEntitiesList() const {
		std::vector<Entity*> entitiesList;
		R_BuildEntitiesList(m_root, entitiesList);
		return entitiesList;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::BeginFrame() {
		for (Actor * actor : m_startingActors) {
			actor->Start();
			m_activeActors.insert(actor);
		}
		m_startingActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Update(const float _delta) {
		for (Actor * actor : m_activeActors) {
			if (actor->IsEnabled()) {
				actor->Update(_delta);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Scene::R_DeleteEntity(Entity* _entity, std::set<Entity*>& _deletedEntitiesSet) {
		if (_entity != nullptr && _deletedEntitiesSet.find(_entity) == _deletedEntitiesSet.end()) {

			const std::vector<Entity*> childs = _entity->GetChilds(); // copy
			for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
				R_DeleteEntity(childs[childIndex], _deletedEntitiesSet);
			}

			if (Engine::GetEngine().GetSelectedentity() == _entity) {
				Engine::GetEngine().Deselect();
			}
			_deletedEntitiesSet.insert(_entity);
			if (_entity->GetParent() != nullptr) {
				_entity->GetParent()->RemoveChild(_entity);
			}
			Debug::Get() << "delete Entity: " << _entity->GetName() << Debug::Endl();
			delete(_entity);
		}
	}

	//================================================================================================================================
	// Deletes every entity in the m_toDeleteLater vector
	//================================================================================================================================
	void Scene::EndFrame() {
		for (Entity * entity : m_outdatedAABB) {
			entity->ComputeAABB();
		} m_outdatedAABB.clear();

		// Delete entities 
		std::set<Entity*> deletedEntitiesSet;
		for (int entityToDeleteIndex = 0; entityToDeleteIndex < m_entitiesToDelete.size(); entityToDeleteIndex++) {
			Entity * entitytoDelete = m_entitiesToDelete[entityToDeleteIndex];
			R_DeleteEntity(entitytoDelete, deletedEntitiesSet);
		}
		m_entitiesToDelete.clear();
	}

	//================================================================================================================================
	// Deletes every entity in the m_toDeleteLater vector
	//================================================================================================================================
	void Scene::OnGui() {
		int nb = (int)m_activeActors.size();
		ImGui::DragInt("nb", &nb);
		for (Actor * actor : m_activeActors)
		{
			ImGui::Text(actor->GetName());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::OnActorAttach(Actor * _actor) {
		assert(m_activeActors.find(_actor) == m_activeActors.end());
		assert(m_startingActors.find(_actor) == m_startingActors.end());

		m_startingActors.insert(_actor);

	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::OnActorDetach(Actor * _actor) {
		const size_t size = m_activeActors.erase(_actor);
		if (size == 0) {
			Debug::Get() << Debug::Severity::warning << " Scene::OnActorDetach Actor not active : " << _actor->GetName() << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Clear() {
		m_path = "";
		std::set<Entity*> deletedEntitiesSet;
		R_DeleteEntity(m_root, deletedEntitiesSet);
		m_startingActors.clear();
		m_activeActors.clear();
		m_entitiesToDelete.clear();
		m_outdatedAABB.clear();
		m_root = nullptr;

		s_onSceneClear.Emmit();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::New() {
		Clear();
		m_root = CreateEntity("root", nullptr);
		m_root->AddComponent<Transform>();
		s_onSceneLoad.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Save() const {
		Debug::Get() << Debug::Severity::log << "saving scene: " << m_name << Debug::Endl();
		std::ofstream outStream(m_path);
		if (outStream.is_open()) {
			outStream << "Entities: 1 { \n";
			Save(outStream, 0);
			outStream << '}';
			outStream.close();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::Save(std::ostream& _out, const int _indentLevel) const {
		return m_root->Save(_out, _indentLevel + 1);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::Load(std::istream & _in) {
		if (!ReadSegmentHeader(_in)) { return false; }
		int nbEntities = 42;
		if (!ReadInteger(_in, nbEntities) || nbEntities != 1) { return false; }
		if (!ReadStartToken(_in)) { return false; }
		for (int entityIndex = 0; entityIndex < nbEntities; entityIndex++) {
			m_root = CreateEntity("root");
			m_root->LoadEntity(_in);
		}
		if (!ReadEndToken(_in)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::LoadFrom(const std::string _path) {
		Clear();
		std::ifstream inStream(_path);
		if (inStream.is_open()) {
			// Load scene
			Debug::Get() << Debug::Severity::log << "loading scene: " << _path << Debug::Endl();
			if (Load(inStream)) {
				Debug::Log("Load success");
				m_path = _path;
				inStream.close();
				s_onSceneLoad.Emmit(this);
				return true;
			}
			else {
				Debug::Get() << Debug::Severity::error << "failed to load scene: " << _path << Debug::Endl();
				m_path = "";
				inStream.close();
				New();
				return false;
			}

		}
		else {
			Debug::Get() << Debug::Severity::error << "failed to open file " << _path << Debug::Endl();
			New();
			return false;
		}
	}
}