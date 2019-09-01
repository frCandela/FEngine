#include "fanGlobalIncludes.h"

#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
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
			, m_path("") {

		}

		//================================================================================================================================
		//================================================================================================================================
		Scene::~Scene() {
			Clear();
		}

		//================================================================================================================================
		//================================================================================================================================
		Gameobject *	Scene::CreateGameobject(const std::string _name) {
			Gameobject* gameObject = new Gameobject(_name);

			gameObject->onComponentCreated.Connect(&Scene::OnComponentCreated, this);
			gameObject->onComponentDeleted.Connect(&Scene::OnComponentDeleted, this);

			m_gameObjects.push_back(gameObject);
			onGameobjectCreated.Emmit(gameObject);

			return gameObject;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Scene::DeleteGameobject(Gameobject* _gameobject) {
			m_gameObjectstoDelete.push_back(_gameobject);
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
		// Deletes every GameObject in the m_toDeleteLater vector
		//================================================================================================================================
		void Scene::EndFrame() {
			for (int gameobjectToDeleteIndex = 0; gameobjectToDeleteIndex < m_gameObjectstoDelete.size(); gameobjectToDeleteIndex++) {
				Gameobject * gameobjecttoDelete = m_gameObjectstoDelete[gameobjectToDeleteIndex];

				for (int gameobjectIndex = 0; gameobjectIndex < m_gameObjects.size(); ++gameobjectIndex) {
					if (m_gameObjects[gameobjectIndex] == gameobjecttoDelete)
					{
						m_gameObjects.erase(m_gameObjects.begin() + gameobjectIndex);

						if (fan::Engine::GetEngine().GetSelectedGameobject() == gameobjecttoDelete) {
							fan::Engine::GetEngine().Deselect();
						}

						delete(gameobjecttoDelete);
						break;
					}
				}
			}
			m_gameObjectstoDelete.clear();
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

			for (int gameobjectIndex = 0; gameobjectIndex < m_gameObjects.size(); gameobjectIndex++) {
				delete m_gameObjects[gameobjectIndex];
			}
			m_gameObjects.clear();
			m_startingActors.clear();
			m_activeActors.clear();
			m_gameObjectstoDelete.clear();
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
				for (int gameobjectIndex = 0; gameobjectIndex < m_gameObjects.size(); gameobjectIndex++) {
					scene::Gameobject * gameobject = m_gameObjects[gameobjectIndex];
					if (gameobject->HasFlag(scene::Gameobject::NOT_SAVED) == false) {
						gameobject->Save(outStream);
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
					if (inputString == "gameobject") {
						inStream >> inputString; // Gameobject name
						scene::Gameobject * gameobject = CreateGameobject(inputString);
						fan::Debug::Get() << fan::Debug::Severity::log << "Gameobject: " << inputString << std::endl;
						gameobject->Load(inStream);
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