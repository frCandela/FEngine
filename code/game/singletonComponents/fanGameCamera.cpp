#include "game/singletonComponents/fanGameCamera.hpp"

#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::CAMERA16;
		_info.init = &GameCamera::Init;
		_info.onGui = &GameCamera::OnGui;
		_info.save = &GameCamera::Save;
		_info.load = &GameCamera::Load;
		_info.name = "game camera";
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		GameCamera& gameCamera = static_cast<GameCamera&>( _component );
		gameCamera.heightFromTarget = 30.f;
		gameCamera.marginRatio = btVector2( 1.f, 1.f );
		gameCamera.minOrthoSize = 15.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		GameCamera& gameCamera = static_cast<GameCamera&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
// 			ImGui::FanComponent( "players manager", m_playersManager );
// 			ImGui::FanComponent( "camera", m_camera );
			ImGui::DragFloat( "height from target", &gameCamera.heightFromTarget, 0.25f, 0.5f, 30.f );
			ImGui::DragFloat2( "margin ratio", &gameCamera.marginRatio[0], 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "minSize", &gameCamera.minOrthoSize, 0.1f, 0.f, 100.f );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::Save( const EcsSingleton& _component, Json& _json )
	{
		const GameCamera& gameCamera = static_cast<const GameCamera&>( _component );
 		Serializable::SaveVec2( _json, "margin_ratio", gameCamera.marginRatio );
 		Serializable::SaveFloat( _json, "min_size", gameCamera.minOrthoSize );
 		Serializable::SaveFloat( _json, "height_from_target", gameCamera.heightFromTarget );
//  		Serializable::SaveComponentPtr( _json, "players_manager", m_playersManager );
//  		Serializable::SaveComponentPtr( _json,"camera", m_camera );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::Load( EcsSingleton& _component, const Json& _json )
	{
		GameCamera& gameCamera = static_cast<GameCamera&>( _component );
		Serializable::LoadVec2( _json, "margin_ratio", gameCamera.marginRatio );
		Serializable::LoadFloat( _json, "min_size", gameCamera.minOrthoSize );
		Serializable::LoadFloat( _json, "height_from_target", gameCamera.heightFromTarget );
		//  		Serializable::SaveComponentPtr( _json, "players_manager", m_playersManager );
		//  		Serializable::SaveComponentPtr( _json,"camera", m_camera );
	}

	//================================================================================================================================
	// creates the game camera entity & components
	// setups the GameCamera singleton
	//================================================================================================================================
	void GameCamera::CreateGameCamera( EcsWorld & _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();
		SceneNode& cameraNode = scene.CreateSceneNode( "game_camera", scene.root );
		const EcsEntity cameraID = _world.GetEntity( cameraNode.handle );
		
		Camera& camera = _world.AddComponent<Camera>( cameraID );
		camera.type = Camera::ORTHOGONAL;

		Transform& transform = _world.AddComponent<Transform>( cameraID );
		transform.SetRotationEuler( btVector3( 90.f, 0.f, 0.f ) );
		transform.SetPosition( btVector3( 0, 5, 0 ) );

		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		gameCamera.cameraNode = &cameraNode;
	}

	//================================================================================================================================
	// deletes the games camera entity & components
	// unregisters is from the GameCamera singleton
	//================================================================================================================================
	void GameCamera::DeleteGameCamera( EcsWorld& _world )
	{
		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		_world.Kill( _world.GetEntity( gameCamera.cameraNode->handle ) );
		gameCamera.cameraNode = nullptr;
	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void CameraController::Start()
// 	{
// 		REQUIRE_TRUE( m_camera.IsValid(), "CameraController missing main camera reference" );
// 		REQUIRE_TRUE( *m_playersManager != nullptr, "CameraController: missing reference to the PlayersManager" )
// 
// 		m_gameobject->GetScene().SetMainCamera( *m_camera );
// 	}
}