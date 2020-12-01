#include "fanGameCamera.hpp"

#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/components/fanCamera.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void GameCamera::SetInfo( EcsSingletonInfo& _info )
	{
		_info.save   = &GameCamera::Save;
		_info.load   = &GameCamera::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void GameCamera::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		GameCamera& gameCamera = static_cast<GameCamera&>( _component );
		gameCamera.mHeightFromTarget = 30.f;
		gameCamera.mMarginRatio      = btVector2( 1.f, 1.f );
		gameCamera.mMinOrthoSize     = 15.f;
	}

	//========================================================================================================
	//========================================================================================================
	void GameCamera::Save( const EcsSingleton& _component, Json& _json )
	{
		const GameCamera& gameCamera = static_cast<const GameCamera&>( _component );
 		Serializable::SaveVec2( _json, "margin_ratio", gameCamera.mMarginRatio );
 		Serializable::SaveFloat( _json, "min_size", gameCamera.mMinOrthoSize );
 		Serializable::SaveFloat( _json, "height_from_target", gameCamera.mHeightFromTarget );
	}

	//========================================================================================================
	//========================================================================================================
	void GameCamera::Load( EcsSingleton& _component, const Json& _json )
	{
		GameCamera& gameCamera = static_cast<GameCamera&>( _component );
		Serializable::LoadVec2( _json, "margin_ratio", gameCamera.mMarginRatio );
		Serializable::LoadFloat( _json, "min_size", gameCamera.mMinOrthoSize );
		Serializable::LoadFloat( _json, "height_from_target", gameCamera.mHeightFromTarget );
	}

	//========================================================================================================
	// creates the game camera entity & components
	// setups the GameCamera singleton
	//========================================================================================================
    GameCamera& GameCamera::CreateGameCamera( EcsWorld & _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();
		SceneNode& cameraNode = scene.CreateSceneNode( "game_camera", &scene.GetRootNode() );
		cameraNode.AddFlag( SceneNode::NoSave | SceneNode::NoDelete | SceneNode::NoRaycast );
		const EcsEntity cameraID = _world.GetEntity( cameraNode.mHandle );
		
		Camera& camera = _world.AddComponent<Camera>( cameraID );
		camera.mType = Camera::Orthogonal;

		Transform& transform = _world.AddComponent<Transform>( cameraID );
		transform.SetRotationEuler( btVector3( 90.f, 0.f, 0.f ) );
		transform.SetPosition( btVector3( 0, 5, 0 ) );

		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		gameCamera.mCameraHandle = cameraNode.mHandle;

		return gameCamera;
	}

	//========================================================================================================
	// deletes the games camera entity & components
	// unregisters is from the GameCamera singleton
	//========================================================================================================
	void GameCamera::DeleteGameCamera( EcsWorld& _world )
	{
		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		_world.Kill( _world.GetEntity( gameCamera.mCameraHandle ) );
		gameCamera.mCameraHandle = 0;
	}
}