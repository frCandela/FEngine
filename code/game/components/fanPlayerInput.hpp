#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"
#include "network/packets/fanPacketPlayerInput.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	class PlayerInput : public Component
	{
	public:
		enum InputType { KEYBOARD_MOUSE, JOYSTICK };

		InputType	GetInputType() const { return m_inputType; }
		void		SetInputType( const InputType _type );
		void		SetJoystickID( const int _joystickID ) { m_joystickID = _joystickID; }
		int			GetJoystickID() const { return m_joystickID; }

		void			 SetInputData( const InputData _inputData ) { m_inputData = _inputData; }
		const InputData& GetInputData() const { return m_inputData; }
		void			 RefreshInput();
		void			 SetReplicated( const bool _isReplicated ) { m_isReplicated = _isReplicated; }


		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		// ISerializable
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;

		DECLARE_TYPE_INFO( PlayerInput, Component );

	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		InputType	m_inputType = KEYBOARD_MOUSE;
		int			m_joystickID = -1;
		float		m_directionCutTreshold = 0.25f;
		bool		m_isReplicated = false;

		InputData	m_inputData;

		std::vector< glm::vec2 > m_directionBuffer;
		btVector3	m_direction;

		glm::vec2	GetDirectionAverage();
		btVector3	GetInputDirection();
		float		GetInputLeft();
		float		GetInputForward();
		float		GetInputBoost();
		float		GetInputFire();
		bool		GetInputStop();
	};
}