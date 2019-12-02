#pragma once

#include "scene/actors/fanActor.h"
#include "network/fanServer.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class GameServer : public Actor
	{
	public:
		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		const Server& GetServer() { return m_server; }

		// ISerializable
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;
		
		DECLARE_TYPE_INFO( GameServer, Component );

	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		Server m_server;

	};
}