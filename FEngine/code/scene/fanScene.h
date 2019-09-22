#pragma once

#include "core/fanSignal.h"
#include "core/fanISerializable.h"

namespace fan
{
	class Entity;
	class Component;
	class Actor;

	//================================================================================================================================
	//================================================================================================================================
	class Scene : public ISerializable
	{
	public:
		static Signal<Scene*>	s_onSceneLoad;
		static Signal<>			s_onSceneClear;

		Scene(const std::string _name);
		~Scene();

		Entity *					CreateEntity(const std::string _name, Entity * _parent = nullptr);	// Creates a game object and adds it to the scene hierarchy
		void						DeleteEntity(Entity* _entity);										// Deletes a entity and removes it from the scene hierarchy at the end of the frame
		std::vector < Entity * >	BuildEntitiesList() const;

		template<typename ComponentType>
		ComponentType * FindComponentOfType() const;

		void BeginFrame();
		void Update(const float _delta);
		void EndFrame();
		void OnGui();

		void New();
		void Save() const;
		bool LoadFrom(const std::string _path);

		void				ComputeAABBEndFrame(Entity * _entity) { m_outdatedAABB.insert(_entity); }
		Entity *			GetRoot() { return m_root; }
		inline std::string	GetName() const { return m_name; }
		bool				HasPath() const { return m_path.empty() == false; }
		inline std::string	GetPath() const { return m_path; }
		void				SetPath(const std::string _path) { m_path = _path; }

	private:
		std::string m_name;
		std::string m_path;
		Entity * m_root;

		std::vector < Entity * >	m_entitiesToDelete;
		std::set< Entity * >	m_outdatedAABB;
		std::set< Actor * >	m_startingActors;
		std::set< Actor * >	m_activeActors;

		void OnActorAttach(Actor * _actor);
		void OnActorDetach(Actor * _actor);

		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;
		void Clear();

		void				R_DeleteEntity(Entity* _entity, std::set<Entity*>&	_deletedEntitiesSet);
		void				R_BuildEntitiesList(Entity* _entity, std::vector<Entity*>& _entitiesList) const;
		Component *	R_FindComponentOfType(Entity * _entity, const uint32_t _typeID) const;
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType * Scene::FindComponentOfType() const {
		static_assert((std::is_base_of<Component, ComponentType>::value));
		return  static_cast<ComponentType*> (R_FindComponentOfType(m_root, ComponentType::s_typeID));
	}
}
