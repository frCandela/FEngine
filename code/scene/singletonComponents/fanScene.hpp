#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "game/fanGameSerializable.hpp"
#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	class Prefab;

	//================================================================================================================================
	//================================================================================================================================
	struct Scene : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );

		~Scene();

		SceneNode& CreateSceneNode( const std::string _name, SceneNode* const _parentNode, const bool _generateID = true );
		SceneNode* CreatePrefab( const Prefab& _prefab, SceneNode* const _parent ); // @todo return an entityID instead of a scene node

		void New();
		void Save() const;
		bool LoadFrom( const std::string _path );
		void Clear();
		void SetMainCamera( SceneNode& _nodeCamera );

		static uint32_t	R_FindMaximumId( SceneNode& _node );
		static void		R_SaveToJson( const SceneNode& _node, Json& _json );
		static void		R_LoadFromJson( const Json& _json, SceneNode& _node, const uint32_t _idOffset );
		static void		RemapSceneNodesIndices( Json& _json );

		Signal< Scene& >		onClear;
		Signal< Scene& >		onLoad;
		Signal< SceneNode* >	onDeleteSceneNode;
		Signal< SceneNode& >	onSetMainCamera;
		Signal< EntityID >		onCreateRigidbody;
		Signal< EntityID >		onDeleteRigidbody;

		EcsWorld*					world;
		std::string					path;		
		SceneNode *					root = nullptr;
		uint32_t					nextUniqueID = 1;
		SceneNode*					mainCamera = nullptr;
	};
}
