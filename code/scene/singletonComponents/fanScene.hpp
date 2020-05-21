#pragma once

#include "core/fanSignal.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	class Prefab;

	//================================================================================================================================
	// contains the scene tree root and a map of scene nodes for fast access
	// also controls the unique ids for the nodes & the scene serialization to json
	//================================================================================================================================
	struct Scene : public EcsSingleton
	{
		ECS_SINGLETON( Scene )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		SceneNode& CreateSceneNode( const std::string _name, SceneNode* const _parentNode, const bool _generateID = true );

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

		EcsWorld* const								world = nullptr;
		std::string									path;		
		SceneNode *									root;
		uint32_t									nextUniqueID;
		SceneNode*									mainCamera;
		std::unordered_map< uint32_t, SceneNode* >  nodes;
	};
}
