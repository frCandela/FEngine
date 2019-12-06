#pragma once

#include "scene/actors/fanActor.h"
#include "scene/fanComponentPtr.h"

namespace fan {

	class Camera;
	class PlayersManager;

	//================================================================================================================================
	//================================================================================================================================	
	class CameraController : public Actor {
	public:
		void Start() override;
		void Stop() override {}
		void Update(const float _delta) override;
		void LateUpdate( const float _delta ) override;

		void OnDetach() override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::CAMERA16; }

		DECLARE_TYPE_INFO(CameraController, Actor );

	protected:
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		Camera *	m_camera;
		ComponentPtr<PlayersManager> m_playersManager;

		float		m_heightFromTarget	= 30.f;
		btVector2	m_marginRatio		= btVector2( 1.f, 1.f );
		float		m_minOrthoSize		= 15.f;
	};
}