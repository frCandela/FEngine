#pragma once

#include "util/fanSignal.h"

namespace scene
{
	class Gameobject;
	class Component;
	class Actor;

	class Scene
	{
	public:
		util::Signal<Gameobject*> onGameobjectCreated;

		Scene(const std::string _name);
		~Scene();

		Gameobject *	CreateGameobject( const std::string _name );	// Creates a game object and adds it to the scene hierarchy
		void			DeleteGameobject( Gameobject* _gameobject);		// Deletes a gameobject and removes it from the scene hierarchy at the end of the frame
		
		void	BeginFrame();
		void	Update(const float _delta);
		void	EndFrame();

		void SaveTo(const std::string _path) const ;
		void LoadFrom(const std::string _path);

		const std::vector<Gameobject *>  & GetGameObjects() const	{ return m_gameObjects; }
		inline std::string GetName() const							{ return m_name; }
	private:
		std::string m_name;

		std::vector <Gameobject*> m_gameObjectstoDelete;
		std::vector<Gameobject *> m_gameObjects;

		std::set< scene::Actor * > m_startingActors;
		std::set< scene::Actor * > m_activeActors;

		void OnComponentCreated(scene::Component * _component );
		void OnComponentDeleted(scene::Component * _component);
	};
}