#include "game/singletonComponents/fanGameCamera.hpp"

#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( GameCamera );

	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::SetInfo( SingletonComponentInfo& _info )
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
	void GameCamera::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		GameCamera& gameCamera = static_cast<GameCamera&>( _component );
		gameCamera.heightFromTarget = 30.f;
		gameCamera.marginRatio = btVector2( 1.f, 1.f );
		gameCamera.minOrthoSize = 15.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameCamera::OnGui( SingletonComponent& _component )
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
	void GameCamera::Save( const SingletonComponent& _component, Json& _json )
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
	void GameCamera::Load( SingletonComponent& _component, const Json& _json )
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
		Scene& scene = _world.GetSingletonComponent<Scene>();
		SceneNode& cameraNode = scene.CreateSceneNode( "game_camera", scene.root );
		const EntityID cameraID = _world.GetEntityID( cameraNode.handle );
		
		Camera& camera = _world.AddComponent<Camera>( cameraID );
		camera.type = Camera::ORTHOGONAL;

		Transform& transform = _world.AddComponent<Transform>( cameraID );
		transform.SetRotationEuler( btVector3( 90.f, 0.f, 0.f ) );

		GameCamera& gameCamera = _world.GetSingletonComponent<GameCamera>();
		gameCamera.cameraNode = &cameraNode;
	}

	//================================================================================================================================
	// deletes the games camera entity & components
	// unregisters is from the GameCamera singleton
	//================================================================================================================================
	void GameCamera::DeleteGameCamera( EcsWorld& _world )
	{
		GameCamera& gameCamera = _world.GetSingletonComponent<GameCamera>();
		_world.KillEntity( _world.GetEntityID( gameCamera.cameraNode->handle ) );
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