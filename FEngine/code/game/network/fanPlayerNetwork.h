#pragma once

#include "scene/actors/fanActor.h"
#include "network/fanClient.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class PlayerNetwork : public Actor
	{
	public:
		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		// ISerializable
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;
		
		DECLARE_TYPE_INFO( PlayerNetwork, Component );

	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		Client m_client;
	};
}