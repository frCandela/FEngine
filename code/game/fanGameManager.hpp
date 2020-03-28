#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================	
	class GameManager// : public Actor
	{
	public:

		void Start() /*override*/;
		void Stop() /*override*/ {}
		void Update( const float _delta ) /*override*/;
		void LateUpdate( const float /*_delta*/ ) /*override*/ {}

		//ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		void OnGui() /*override*/;
	protected:
		bool Load( const Json& _json ) /*override*/;
		bool Save( Json& _json ) const /*override*/;

		void OnAttach() /*override*/;
		void OnDetach() /*override*/;
// 
// 	private:
// 		ComponentPtr<Camera> m_gameCamera;
	};
}