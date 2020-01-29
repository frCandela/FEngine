#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"
#include "core/fanSignal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Actor : public Component
	{
	public:
		enum State{ STOPPED, STARTING, ACTIVE, PAUSED };

		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Update(const float _delta) = 0;
		virtual void LateUpdate( const float _delta ) = 0;
		virtual void OnEnable() {};
		virtual void OnDisable() {};

		bool IsActor() const override { return true; }
		void OnGui() override;
		State GetState() const { return m_state; }
		void  SetState( const State _state ) { m_state = _state; }
		bool IsEnabled() const { return m_state == State::ACTIVE; }
		void SetEnabled( const bool _enabled );


		DECLARE_ABSTRACT_TYPE_INFO( Actor, Component );
	protected:

		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		State m_state = State::STOPPED;
	};
}