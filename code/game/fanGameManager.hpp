#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/actors/fanActor.h"
#include "scene/fanComponentPtr.h"

namespace fan {

	class Camera;

	//================================================================================================================================
	//================================================================================================================================	
	class GameManager : public Actor {
	public:

		void Start() override;
		void Stop() override {}
		void Update(const float _delta) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		void OnGui() override;

		DECLARE_TYPE_INFO(GameManager, Actor );
	protected:
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void OnAttach() override;
		void OnDetach() override;

	private:
		ComponentPtr<Camera> m_gameCamera;
	};
}