#pragma once

#include "scene/actors/fanActor.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class WithEnergy : public Actor
	{
	public:
		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;

		bool  TryRemoveEnergy( const float _energyConsumed );
		void  AddEnergy( const float _energyAdded );
		float GetEnergy( ) const { return m_currentEnergy; }

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_TYPE_INFO( WithEnergy, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_currentEnergy = 0.f;
		float m_maxEnergy = 100.f;
	};
}