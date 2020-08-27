#include "scene/singletons/fanScene.hpp"

#include <stack>
#include <fstream>
#include "core/fanDebug.hpp"
#include "network/singletons/fanTime.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/time/fanProfiler.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Scene::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::SCENE16;
		_info.group = EngineGroups::Scene;
		_info.onGui = &Scene::OnGui;
		_info.name = "scene";
		_info.mSlots.push_back( new Slot<>("Test1", &Scene::Test1 ));
        _info.mSlots.push_back( new Slot<>("Test2", &Scene::Test2 ));
        _info.mSlots.push_back( new Slot<int>("Test3", &Scene::Test3 ));
	}

	//========================================================================================================
	//========================================================================================================
	void Scene::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		Scene& scene = static_cast<Scene&>( _component );
		scene.mPath             = "";
		scene.mRootNodeHandle   = 0;
		scene.mMainCameraHandle = 0;
		scene.mNodes.clear();
		EcsWorld*& worldNoConst = const_cast<EcsWorld*&>( scene.mWorld );
		worldNoConst = &_world;
	}

    //========================================================================================================
    //========================================================================================================
    void Scene::Test1( EcsSingleton&  )
    {
        Debug::Log("scene test slot 1");
    }
    //========================================================================================================
    //========================================================================================================
    void Scene::Test2( EcsSingleton&  )
    {
        Debug::Log("scene test slot 2");
    }

    //========================================================================================================
    //========================================================================================================
    void Scene::Test3( EcsSingleton& , int _test  )
    {
        Debug::Log() << "scene test slot 3 " << _test << Debug::Endl();
    }

	//========================================================================================================
	//========================================================================================================
	SceneNode& Scene::GetRootNode() const
	{
		return mWorld->GetComponent<SceneNode>( mWorld->GetEntity( mRootNodeHandle ) );
	}

	//========================================================================================================
	// _handle can be used to force the handle of scene node entity,
	// if _handle=0 (by default), generate a new handle
	//========================================================================================================
	SceneNode& Scene::CreateSceneNode( const std::string _name, SceneNode* const _parentNode, EcsHandle _handle )
	{		
		assert( _parentNode != nullptr || mRootNodeHandle == 0 ); // we can have only one root node

		EcsEntity entity = mWorld->CreateEntity();

		// set entity handle
		EcsHandle handle = 0;
		if( _handle == 0 )
		{
			handle = mWorld->AddHandle( entity );
		}
		else
		{
			mWorld->SetHandle( entity, _handle );
			handle = _handle;
		}
		assert( mNodes.find( handle ) == mNodes.end() );
		mNodes.insert( handle );

		SceneNode& sceneNode = mWorld->AddComponent<SceneNode>( entity );
		mWorld->AddComponent<Bounds>( entity );
		sceneNode.AddFlag( SceneNode::BoundsOutdated );
		if( _parentNode == nullptr ) // root node
		{
			sceneNode.AddFlag( SceneNode::NoRaycast | SceneNode::NoDelete );
		}

		sceneNode.Build( _name, *this, handle, _parentNode );
		return sceneNode;
	}

	//========================================================================================================
	//========================================================================================================
	EcsHandle Scene::R_FindMaximumHandle( SceneNode& _node )
	{
		EcsWorld& world = *_node.scene->mWorld;

		EcsHandle handle = _node.handle;
		const std::vector<EcsHandle>& childs = _node.childs;
		for ( int childIndex = 0; childIndex < (int)childs.size(); childIndex++ )
		{
		    SceneNode& node = world.GetComponent<SceneNode>( world.GetEntity( childs[childIndex] ) );
			EcsHandle childHandle = R_FindMaximumHandle( node );
			if ( childHandle > handle )
			{
				handle = childHandle;
			}
		}
		return handle;
	}

	//========================================================================================================
	//========================================================================================================
	void Scene::SetMainCamera( const EcsHandle _cameraHandle )
	{
		if( _cameraHandle != mMainCameraHandle )
		{
            mMainCameraHandle = _cameraHandle;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Scene::Clear()
	{
		mOnClear.Emmit( *this);
		mPath = "";
		mWorld->Clear();
		mNodes.clear();
        mRootNodeHandle   = 0;
        mMainCameraHandle = 0;
        ScenePointers::Clear( mWorld->GetSingleton<ScenePointers>() );
	}

	//========================================================================================================
	//========================================================================================================
	void Scene::New()
	{
		Clear( );
		SceneNode& rootNode = CreateSceneNode( "root", nullptr );
        mRootNodeHandle = rootNode.handle;
		mOnLoad.Emmit( *this );
		mWorld->ApplyTransitions();
	}

	//========================================================================================================
	//========================================================================================================
	void Scene::Save( ) const
	{
		std::ofstream outStream( mPath );
		if ( outStream.is_open() )
		{
			Json json;

			// scene global parameters
			Json& jScene = json["scene"];
			{
				Serializable::SaveString( jScene, "path", mPath );
			}

			// save singleton components
			Json& jSingletons = jScene["singletons"];
			unsigned nextIndex = 0;
			const std::vector< EcsSingletonInfo >& singletonsInfos = mWorld->GetVectorSingletonInfo();
			for ( const EcsSingletonInfo& info : singletonsInfos )
			{
				if( info.save != nullptr )
				{
					Json& jSingleton_i = jSingletons[nextIndex++];
					Serializable::SaveUInt( jSingleton_i, "singleton_id", info.type);
					Serializable::SaveString( jSingleton_i, "singleton", info.name );
					EcsSingleton& singleton = mWorld->GetSingleton( info.type);
					info.save( singleton, jSingleton_i );
				}
			}

			// saves all scene nodes recursively
			Json& jRoot = jScene["root"];
			R_SaveToJson( GetRootNode(), jRoot );
			RemapSceneNodesIndices( jRoot );
			outStream << json; // write to disk			
			outStream.close();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Scene::R_SaveToJson( const SceneNode& _node, Json& _json )
	{	
		EcsWorld& world = *_node.scene->mWorld;
		Serializable::SaveString( _json, "name", _node.name );
		Serializable::SaveUInt( _json, "handle", _node.handle );

		// save components
		Json& jComponents = _json["components"];
		{
			EcsEntity entity = world.GetEntity( _node.handle );
			unsigned nextIndex = 0;
			for( const EcsComponentInfo& info : world.GetComponentInfos() )
			{
				if( ! world.HasComponent( entity, info.type ) ) { continue; }

				// if a save method is provided, saves the component
				EcsComponent& component = world.GetComponent( entity, info.type );								
				if( info.save != nullptr )
				{
					Json& jComponent_i = jComponents[nextIndex++];
					Serializable::SaveUInt( jComponent_i, "component_type", info.type);
					Serializable::SaveString( jComponent_i, "type_name", info.name );
					info.save( component, jComponent_i );
				}				
			}
		}

		// save childs
		Json& jchilds = _json["childs"];
		unsigned childIndex = 0;
		for( int sceneNodeIndex = 0; sceneNodeIndex < (int)_node.childs.size(); sceneNodeIndex++ )
		{
			const EcsHandle childHandle = _node.childs[sceneNodeIndex];
			SceneNode& childNode = world.GetComponent<SceneNode>( world.GetEntity(childHandle ));
			if( ! childNode.HasFlag( SceneNode::NoSave ) )
			{
				Json& jchild = jchilds[childIndex];
				R_SaveToJson( childNode, jchild );
				++childIndex;
			}
		}		
	}

	//========================================================================================================
	// Find all the gameobjects handles in the json and remap them on a range close to zero
	// ex: 400, 401, 1051 will be remapped to 1,2,3
	//========================================================================================================
	void Scene::RemapSceneNodesIndices( Json& _json )
	{
		std::vector< Json* > jsonIndices;
		std::set< uint32_t > uniqueIndices;

		// parse all the json, get all the scene nodes IDs & generates a remap table
		std::map< uint32_t, uint32_t > remapTable;
		{
			int nextRemapIndex = 1;
			std::stack< const Json* > stack;
			stack.push( &_json );
			while( !stack.empty() )
			{
				const Json& jNode = *stack.top();
				stack.pop();

				uint32_t id = jNode["handle"];
				remapTable[id] = nextRemapIndex++;

				// push all childs
				const Json& jchilds = jNode["childs"];
				{
					for( int childIndex = 0; childIndex < (int)jchilds.size(); childIndex++ )
					{
						const Json& jChild = jchilds[childIndex];
						stack.push( &jChild );
					}
				}
			}
		}

		// remap all indices ( scene nodes & component pointers )
		{
			std::stack< Json* > stack;
			stack.push( &_json );
			while( !stack.empty() )
			{
				Json& js = *stack.top();
				stack.pop();

				// remap
				Json::iterator jNodeId = js.find( "handle" );
				if( jNodeId != js.end() )
				{
 					const uint32_t nodeId = *jNodeId;
					auto it = remapTable.find( nodeId );
					if( nodeId == 0 ) {}
 					else if( it != remapTable.end() )
					{
						const uint32_t remapId = it->second;
						*jNodeId = remapId;
					}
					else
					{						
						*jNodeId = 0;// pointer target is outside of the json nodes
					}
				}

				// push all childs
				if( js.is_structured() )
				{
					for( auto& element : js )
					{
						stack.push( &element );
					}
				}
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	bool Scene::LoadFrom( const std::string _path )
	{
		Clear();
 		std::ifstream inStream( _path );
 		if ( inStream.is_open() && inStream.good() )
 		{
 			// Load scene
			Json sceneJson;
			inStream >> sceneJson;

			// scene global parameters
			const Json& jScene = sceneJson["scene"];
			{
				Serializable::LoadString( jScene, "path", mPath );
			}

			// load singleton components
			if( jScene.find( "singletons" ) != jScene.end() )
			{
				const Json& jSingletons = jScene["singletons"];
				for( int childIndex = 0; childIndex < (int)jSingletons.size(); childIndex++ )
				{
					const Json& jSingleton_i = jSingletons[childIndex];
					unsigned staticIndex = 0;
					Serializable::LoadUInt( jSingleton_i, "singleton_id", staticIndex );
					const EcsSingletonInfo* info = mWorld->SafeGetSingletonInfo( staticIndex );
					if( info != nullptr && info->load != nullptr )
					{
						EcsSingleton& singleton = mWorld->GetSingleton( staticIndex );
						info->load( singleton, jSingleton_i );
					}
					else
					{
                        Debug::Error()  << "corrupted singleton component with id "
                                        << staticIndex << Debug::Endl();
					}
				}
			}			

			// loads all nodes recursively
			const Json& jRoot = jScene["root"];
			const EcsHandle handleOffset = 0; 
			SceneNode&  rootNode = R_LoadFromJson( jRoot, *this, nullptr, handleOffset );
            mRootNodeHandle = rootNode.handle;

			mPath = _path;
			inStream.close();
			const EcsHandle maxHandle = R_FindMaximumHandle( rootNode ) + 1;
			mWorld->SetNextHandle( maxHandle );

			ScenePointers::ResolveComponentPointers( *mWorld, handleOffset );
			mWorld->Run<SInitFollowTransforms>();

			mOnLoad.Emmit( *this );
			mWorld->ApplyTransitions();
			return true;
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "failed to open file " << _path << Debug::Endl();
			New();
			return false;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
    SceneNode& Scene::R_LoadFromJson( const Json& _json,
                                      Scene& _scene,
                                      SceneNode* _parent,
                                      const uint32_t _handleOffset )
	{
		//ScopedTimer timer("load scene");
		EcsWorld& world = *_scene.mWorld;

		EcsHandle nodeHandle;
		Serializable::LoadUInt( _json, "handle", nodeHandle );
		SceneNode& node = _scene.CreateSceneNode( "tmp", _parent, nodeHandle + _handleOffset );
		Serializable::LoadString( _json, "name", node.name );

		// append id		
		_scene.mNodes.insert( node.handle);

		// components
		const Json& jComponents = _json["components"];
		{
			const EcsEntity	entity = world.GetEntity( node.handle );
			for( int childIndex = 0; childIndex < (int)jComponents.size(); childIndex++ )
			{
				const Json& jComponent_i = jComponents[childIndex];				
				unsigned staticIndex = 0;
				Serializable::LoadUInt( jComponent_i, "component_type", staticIndex );
                const EcsComponentInfo* info		= world.SafeGetComponentInfo( staticIndex );
                if( info != nullptr )
                {
                    EcsComponent& component = world.AddComponent( entity, staticIndex );
                    info->load( component, jComponent_i );
                }
			}
		}

		// Load childs
		const Json& jchilds = _json["childs"];
		{
			for( int childIndex = 0; childIndex < (int)jchilds.size(); childIndex++ )
			{
				const Json& jchild_i = jchilds[childIndex];
				{					
					R_LoadFromJson( jchild_i, _scene, &node, _handleOffset );
				}
			}
		}

		return node;
	}

    //========================================================================================================
    //========================================================================================================
    void Scene::OnGui( EcsWorld& _world, EcsSingleton& _component )
    {
        Scene& scene = static_cast<Scene&>( _component );
        ImGui::Text( "path: %s", scene.mPath.c_str() );

        if( ImGui::CollapsingHeader( "nodes" ) )
        {
            for( EcsHandle handle : scene.mNodes )
            {
                SceneNode& sceneNode = _world.GetComponent<SceneNode>( _world.GetEntity( handle ) );
                ImGui::Text( "%s : %d", sceneNode.name.c_str(), sceneNode.handle );
            }
        }
    }
}