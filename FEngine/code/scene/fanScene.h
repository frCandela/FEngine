#pragma once

namespace scene
{
	class Gameobject;

	class Scene
	{
	public:
		util::Signal<Gameobject*> onGameobjectCreated;

		Scene(const std::string _name);

		Gameobject *	CreateGameobject( const std::string _name );	// Creates a game object and adds it to the scene hierarchy
		void			DeleteGameobject( Gameobject* _gameobject);		// Deletes a gameobject and removes it from the scene hierarchy at the end of the frame
		void			EndFrame();

		const std::vector<Gameobject *>  & GetGameObjects() const	{ return m_gameObjects; }
		inline std::string GetName() const							{ return m_name; }



	private:
		std::string m_name;

		std::vector <Gameobject*> m_gameObjectstoDelete;
		std::vector<Gameobject *> m_gameObjects;
	};
}