#pragma once

#include "game/fanGamePrecompiled.hpp"


namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	class SunLight// : public Actor
	{
	public:
		void Start() /*override*/;
		void Stop() /*override*/ {}
		void Update( const float _delta ) /*override*/;
		void LateUpdate( const float _delta ) /*override*/;

		void OnGui() /*override*/;
		//ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

	protected:

		void OnAttach() /*override*/;
		void OnDetach() /*override*/;
		bool Load( const Json& _json ) /*override*/;
		bool Save( Json& _json ) const /*override*/;

	private:
	};
}