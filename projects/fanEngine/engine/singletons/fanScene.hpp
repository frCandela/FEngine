#pragma once

#include <set>
#include "core/ecs/fanSignal.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	class Prefab;

	//========================================================================================================
	// contains the scene tree root and a map of scene nodes for fast access
	// also controls the unique ids for the nodes & the scene serialization to json
	//========================================================================================================
	struct Scene : public EcsSingleton
	{
		ECS_SINGLETON( Scene )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

        SceneNode& CreateSceneNode( const std::string _name,
                                    SceneNode* const _parentNode,
                                    EcsHandle _handle = 0 );

		void New();
		void Save() const;
		bool LoadFrom( const std::string _path );
		void Clear();
		void SetMainCamera( const EcsHandle _cameraHandle );

		static EcsHandle  RFindMaximumHandle( SceneNode& _node );
		static void		  RSaveToJson( const SceneNode& _node, Json& _json );
        static SceneNode& RLoadFromJson( const Json& _json, Scene& _scene, SceneNode* _parent, const uint32_t _handleOffset );

        using RemapTable = std::map< EcsHandle , EcsHandle >;
        static void GenerateRemapTable( Json& _jsonRootSceneNode, RemapTable& _outRemapTable );
		static void	RemapHandlesRecursively( Json& _json, const RemapTable& _remapTable );

		Signal< Scene& >     mOnClear;
		Signal< Scene& >     mOnLoad;
		Signal< SceneNode* > mOnDeleteSceneNode;
        Signal<>             mOnEditorUseProjectCamera;

		EcsWorld* const		mWorld = nullptr;
		std::string         mPath;
		EcsHandle           mRootNodeHandle;
		EcsHandle           mMainCameraHandle;
		std::set<EcsHandle> mNodes;

		SceneNode& GetRootNode() const;
	};
}
