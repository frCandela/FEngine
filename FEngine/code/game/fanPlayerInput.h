#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class PlayerInput : public Component
	{
	public:
		enum InputType { KEYBOARD_MOUSE, JOYSTICK };

		InputType	GetInputType() const { return m_inputType; }
		void		SetInputType(const InputType _type );
		
		// Generic input
		btVector3	GetInputDirection();
		float		GetInputLeft();
		float		GetInputForward();
		float		GetInputBoost();
		float		GetInputFire();
		bool		GetInputStop();

		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		// ISerializable
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

		DECLARE_TYPE_INFO( PlayerInput, Component );

	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		InputType m_inputType = KEYBOARD_MOUSE;

		btVector3	m_lastDirection;
		float		m_directionCutTreshold = 0.25f;

		std::vector< glm::vec2 > m_directionBuffer;
		glm::vec2 GetDirectionAverage() ;
	};
}