#pragma once

#include "scene/actors/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {

	class Camera;
	class FPSCamera;
	class SpaceShip;

	//================================================================================================================================
	//================================================================================================================================	
	class GameManager : public Actor {
	public:

		void Start() override;
		void Update(const float _delta) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnScenePlay();
		void OnScenePause();

		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }


		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(GameManager)
		DECLARE_TYPE_INFO(GameManager, Actor );
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

		void OnAttach() override;
		void OnDetach() override;

	private:
		Camera * m_camera;
		SpaceShip * m_spaceShip;
	};
}