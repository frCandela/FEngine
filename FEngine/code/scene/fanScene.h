#pragma once

#include "core/fanSignal.h"
#include "core/fanISerializable.h"

namespace fan
{
	namespace scene
	{
		class Entity;
		class Component;
		class Actor;

		//================================================================================================================================
		//================================================================================================================================
		class Scene : public ISerializable
		{
		public:
			fan::Signal<Entity*> onEntityCreated;
			fan::Signal<Scene*> onSceneLoad;

			Scene(const std::string _name);
			~Scene();

			Entity *	CreateEntity(const std::string _name, Entity * _parent = nullptr );	// Creates a game object and adds it to the scene hierarchy
			void		DeleteEntity(Entity* _entity);		// Deletes a entity and removes it from the scene hierarchy at the end of the frame
			std::vector < Entity * > BuildEntitiesList() const;

			void	BeginFrame();
			void	Update(const float _delta);
			void	EndFrame();

			void New();
			void Save() const;

			bool LoadFrom(const std::string _path);

			Entity *						GetRoot() { return m_root;  }
			inline std::string				GetName() const { return m_name; }
			bool							HasPath() const { return m_path.empty() == false; }
			inline std::string				GetPath() const { return m_path; }
			void							SetPath(const std::string _path) { m_path = _path; }

		private:
			std::string m_name;
			std::string m_path;
			Entity * m_root;

			std::vector < Entity * > m_entitiesToDelete;

			std::set< scene::Actor * > m_startingActors;
			std::set< scene::Actor * > m_activeActors;

			void OnActorAttach(scene::Actor * _actor);
			void OnActorDetach(scene::Actor * _actor);

			bool Load(std::istream& _in) override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;
			void R_DeleteEntity		( Entity* _entity, std::set<Entity*>&	_deletedEntitiesSet);
			void R_BuildEntitiesList( Entity* _entity, std::vector<Entity*>& _entitiesList) const ;
			void Clear();
		};
	}
}		
