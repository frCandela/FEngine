#pragma once

#include <set>
#include "ecs/fanSignal.hpp"
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
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		SceneNode& CreateSceneNode( const std::string _name, SceneNode* const _parentNode, EcsHandle _handle = 0 );

		void New();
		void Save() const;
		bool LoadFrom( const std::string _path );
		void Clear();
		void SetMainCamera( const EcsHandle _cameraHandle );

		static EcsHandle	R_FindMaximumHandle( SceneNode& _node );
		static void			R_SaveToJson( const SceneNode& _node, Json& _json );
		static SceneNode&   R_LoadFromJson( const Json& _json, Scene& _scene, SceneNode* _parent, const uint32_t _handleOffset );
		static void			RemapSceneNodesIndices( Json& _json );

		Signal< Scene& >     mOnClear;
		Signal< Scene& >     mOnLoad;
		Signal< SceneNode* > mOnDeleteSceneNode;

		EcsWorld* const		mWorld = nullptr;
		std::string         mPath;
		EcsHandle           mRootNodeHandle;
		EcsHandle           mMainCameraHandle;
		std::set<EcsHandle> mNodes;

		SceneNode& GetRootNode() const;

		// slots
		static void Test1( EcsSingleton& _singleton );
        static void Test2( EcsSingleton& _singleton );
		static void Test3( EcsSingleton& _singleton, int _test  );
	};
}
