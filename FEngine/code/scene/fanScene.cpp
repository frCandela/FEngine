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
	namespace scene {

		//================================================================================================================================
		//================================================================================================================================
		Scene::Scene(const std::string _name) :
			m_name(_name)
			, m_path("") 
			, m_root(nullptr) {
			m_root = CreateEntity("root", nullptr);
			m_root->AddComponent<scene::Transform>();
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

			entity->onComponentCreated.Connect(&Scene::OnComponentCreated, this);
			entity->onComponentDeleted.Connect(&Scene::OnComponentDeleted, this);

			m_entities.push_back(entity);
			onEntityCreated.Emmit(entity);

			return entity;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::DeleteEntity(Entity* _entity) {
			m_entitiesToDelete.push_back(_entity);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::BeginFrame() {
			for (scene::Actor * actor : m_startingActors) {
				actor->Start();
				m_activeActors.insert(actor);
			}
			m_startingActors.clear();
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::Update(const float _delta) {
			for (scene::Actor * actor : m_activeActors) {
				actor->Update(_delta);
			}
		}

		//================================================================================================================================
		// Deletes every entity in the m_toDeleteLater vector
		//================================================================================================================================
		void Scene::EndFrame() {
			for (int entityToDeleteIndex = 0; entityToDeleteIndex < m_entitiesToDelete.size(); entityToDeleteIndex++) {
				Entity * entitytoDelete = m_entitiesToDelete[entityToDeleteIndex];

				for (int entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex) {
					if (m_entities[entityIndex] == entitytoDelete)
					{
						m_entities.erase(m_entities.begin() + entityIndex);

						if (fan::Engine::GetEngine().GetSelectedentity() == entitytoDelete) {
							fan::Engine::GetEngine().Deselect();
						}

						delete(entitytoDelete);
						break;
					}
				}
			}
			m_entitiesToDelete.clear();
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::OnComponentCreated(scene::Component * _component) {

			if (_component->IsActor()) {
				scene::Actor * actor = static_cast<scene::Actor*>(_component);

				assert(m_activeActors.find(actor) == m_activeActors.end());
				assert(m_startingActors.find(actor) == m_startingActors.end());

				m_startingActors.insert(actor);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::OnComponentDeleted(scene::Component * _component) {
			if (_component->IsActor()) {
				scene::Actor * actor = static_cast<scene::Actor*>(_component);
				auto it = m_activeActors.find(actor);
				assert(it != m_activeActors.end());

				m_activeActors.erase(actor);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::Clear() {
			m_path = "";

			for (int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
				delete m_entities[entityIndex];
			}
			m_entities.clear();
			m_startingActors.clear();
			m_activeActors.clear();
			m_entitiesToDelete.clear();
		}


		//================================================================================================================================
		//================================================================================================================================
		void Scene::New() {
			Clear();
			onSceneLoad.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::Save() const {
			fan::Debug::Get() << fan::Debug::Severity::log << "saving scene: " << m_name << std::endl;
			std::ofstream outStream(m_path);
			if (outStream.is_open()) {
				for (int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
					scene::Entity * entity = m_entities[entityIndex];
					if (entity->HasFlag(scene::Entity::NOT_SAVED) == false) {
						entity->Save(outStream);
					}
				}
				outStream.close();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::LoadFrom(const std::string _path) {
			Clear();

			fan::Debug::Get() << fan::Debug::Severity::log << "loading scene: " << _path << std::endl;
			std::ifstream inStream(_path);
			if (inStream.is_open()) {
				m_path = _path;

				std::string inputString = "";
				inStream >> inputString;
				while (inStream.eof() == false) {
					if (inputString == "entity") {
						inStream >> inputString; // entity name
						scene::Entity * entity = CreateEntity(inputString);
						fan::Debug::Get() << fan::Debug::Severity::log << "entity: " << inputString << std::endl;
						entity->Load(inStream);
					}
					else {
						fan::Debug::Get() << fan::Debug::Severity::error << "fail " << inputString << std::endl;
					}
					inStream >> inputString;
				}
				inStream.close();

				onSceneLoad.Emmit(this);
			}
			else {
				fan::Debug::Get() << fan::Debug::Severity::error << "failed to open file " << _path << std::endl;
			}
		}
	}
}