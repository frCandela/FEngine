#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"

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
		ImGui::IconType GetIcon() const override { return ImGui::IconType::ENERGY16; }

		DECLARE_TYPE_INFO( WithEnergy, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_currentEnergy = 0.f;
		float m_maxEnergy = 100.f;
	};
}