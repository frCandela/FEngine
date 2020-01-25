#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Health : public Component
	{
	public:
		Signal<> onFallToZero;

		bool  TryRemoveHealth( const float _healthConsumed );
		void  AddHealth( const float _healthAdded );
		float GetHealth() const { return m_currentHealth; }
		float GetMaxHealth( ) const { return m_maxHealth; }

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::HEART16; }

		DECLARE_TYPE_INFO( Health, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		bool  m_invincible = false;;
 		float m_currentHealth = 0.f;
 		float m_maxHealth = 100.f;
	};
}