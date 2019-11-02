#pragma once

#include "scene/actors/fanActor.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Weapon : public Actor
	{
	public:
		void Start() override;
		void Update( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_EDITOR_COMPONENT( Weapon )
		DECLARE_TYPE_INFO( Weapon, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_lifeTime = 1.f;
		float m_scale = 0.2f;
		float m_speed = 0.2f;
		int   m_bulletsPerFrame = 1;
		btVector3 m_offset ;
	};
}