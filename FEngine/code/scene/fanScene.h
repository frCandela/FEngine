#pragma once

#include "core/fanSignal.h"
#include "core/fanISerializable.h"

namespace fan
{
	class Gameobject;
	class Component;
	class Actor;
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================
	class Scene : public ISerializable
	{
	public:
		friend class Engine;

		Signal<Scene*>	onSceneLoad;
		Signal<>		onSceneClear;
		Signal< Gameobject *>		onDeleteGameobject;

		Scene(const std::string _name);
		~Scene();

		Gameobject *					CreateGameobject(const std::string _name, Gameobject * _parent = nullptr);	// Creates a game object and adds it to the scene hierarchy
		void							DeleteGameobject(Gameobject* _gameobject);										// Deletes a gameobject and removes it from the scene hierarchy at the end of the frame
		std::vector < Gameobject * >	BuildEntitiesList() const;

		template<typename ComponentType>
		ComponentType * FindComponentOfType() const;

		void BeginFrame();
		void Update(const float _delta);
		void EndFrame();
		void OnGui();

		void New();
		void Save() const;
		bool LoadFrom(const std::string _path);

		void				ComputeAABBEndFrame(Gameobject * _gameobject) { m_outdatedAABB.insert(_gameobject); }
		Gameobject *		GetRoot() { return m_root; }
		inline std::string	GetName() const { return m_name; }
		bool				HasPath() const { return m_path.empty() == false; }
		inline std::string	GetPath() const { return m_path; }
		void				SetPath(const std::string _path) { m_path = _path; }
		EcsManager *		GetEcsManager() const { return m_ecsManager; }

	private:
		std::string m_name;
		std::string m_path;
		Gameobject * m_root;
		EcsManager * m_ecsManager = nullptr;

		std::vector < Gameobject * >	m_entitiesToDelete;
		std::set< Gameobject * >	m_outdatedAABB;
		std::set< Actor * >	m_startingActors;
		std::set< Actor * >	m_activeActors;

		void OnActorAttach(Actor * _actor);
		void OnActorDetach(Actor * _actor);

		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;
		void Clear();

		void		R_DeleteGameobject(Gameobject* _gameobject, std::set<Gameobject*>&	_deletedEntitiesSet);
		void		R_BuildEntitiesList(Gameobject* _gameobject, std::vector<Gameobject*>& _entitiesList) const;
		Component *	R_FindComponentOfType(Gameobject * _gameobject, const uint32_t _typeID) const;
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType * Scene::FindComponentOfType() const {
		static_assert((std::is_base_of<Component, ComponentType>::value));
		return  static_cast<ComponentType*> (R_FindComponentOfType(m_root, ComponentType::s_typeID));
	}
}
