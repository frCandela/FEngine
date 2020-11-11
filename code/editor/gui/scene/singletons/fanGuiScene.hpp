#prama once

#include "scene/singletons/fanScene.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Scene::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Scene16;
		_info.mGroup = EngineGroups::Scene;
		_info.onGui  = &Scene::OnGui;
		_info.mName  = "scene";
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
        fanAssert( _parentNode != nullptr || mRootNodeHandle == 0 ); // we can have only one root node

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
        fanAssert( mNodes.find( handle ) == mNodes.end() );
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
    void Scene::OnGui( EcsWorld& _world, EcsSingleton& _component )
    {
        Scene& scene = static_cast<Scene&>( _component );
        ImGui::Text( "path: %s", scene.mPath.c_str() );

        if( ImGui::CollapsingHeader( "nodes" ) )
        {
            for( EcsHandle handle : scene.mNodes )
            {
                SceneNode& sceneNode = _world.GetComponent<SceneNode>( _world.GetEntity( handle ) );
                ImGui::Text( "%s : %d", sceneNode.mName.c_str(), sceneNode.mHandle );
            }
        }
    }
}