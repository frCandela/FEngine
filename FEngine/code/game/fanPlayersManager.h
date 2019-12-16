#pragma once

#include "scene/actors/fanActor.h"

namespace fan
{
	class PlayerNetwork;

	//================================================================================================================================
	//================================================================================================================================	
	class PlayersManager : public Actor
	{
	private:
		//================================================================	
		//================================================================
		struct PlayerData
		{
			Gameobject*		spaceship	= nullptr;
			Gameobject*		persistent	= nullptr;
			PlayerNetwork *	connection	= nullptr;
			std::string		name = "";
		};

	public:

		std::vector< Gameobject * > GetPlayers() const;

		void Start() override;
		void Stop() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void AddPlayer( const int _ID, const std::string& _name );
		void SpawnSpaceShips();

		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		void OnGui() override;

		DECLARE_TYPE_INFO( PlayersManager, Actor );
	protected:
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void OnAttach() override;
		void OnDetach() override;
	private:
		PrefabPtr m_playerPrefab;

		static const int s_mousePlayerID = -1;
		std::map< int, PlayerData > m_players;

		void RemovePlayer( const int _ID );

		void OnJoystickConnect( int _joystickID, bool _connected );
		void OnPlayerDie( Gameobject * _gameobject );
	};
}