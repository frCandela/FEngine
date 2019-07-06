#include "fanIncludes.h"

#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "util/fanSignal.h"
#include "fanEngine.h"


namespace scene {

	//================================================================================================================================
	//================================================================================================================================
	Scene::Scene(const std::string _name) :
		m_name( _name ){

	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject *	Scene::CreateGameobject(const std::string _name) {
		Gameobject* gameObject = new Gameobject(_name);

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
	void Scene::EndFrame() {
		// Deletes every GameObject in the m_toDeleteLater vector
		for (int gameobjectToDeleteIndex = 0; gameobjectToDeleteIndex < m_gameObjectstoDelete.size(); gameobjectToDeleteIndex++) {
			Gameobject * gameobjecttoDelete = m_gameObjectstoDelete[gameobjectToDeleteIndex];

			for (int gameobjectIndex = 0; gameobjectIndex < m_gameObjects.size(); ++gameobjectIndex) {
				if ( m_gameObjects[gameobjectIndex] == gameobjecttoDelete)
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

}