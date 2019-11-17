#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class WithEnergy : public Component
	{
	public:

		bool  TryRemoveEnergy( const float _energyConsumed );
		void  AddEnergy( const float _energyAdded );
		float GetEnergy( ) const { return m_currentEnergy; }
		float GetMaxEnergy( ) const { return m_maxEnergy; }

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_TYPE_INFO( WithEnergy, Component );
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