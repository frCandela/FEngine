#include "fanGlobalIncludes.h"
#include "game/fanSpaceShip.h"

#include "core/time/fanTime.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanRigidbody.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/actors/fanParticleSystem.h"
#include "scene/fanGameobject.h"
#include "core/input/fanMouse.h"
#include "core/math/shapes/fanRay.h"
#include "renderer/fanRenderer.h"
#include "game/fanWithEnergy.h"
#include "game/fanPlayerInput.h"

namespace fan {
	REGISTER_TYPE_INFO(SpaceShip, TypeInfo::Flags::EDITOR_COMPONENT)

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Start() {
		REQUIRE_COMPONENT( WithEnergy, m_energy )
		REQUIRE_COMPONENT( Rigidbody, m_rigidbody )
		REQUIRE_COMPONENT( PlayerInput, m_input );

		REQUIRE_TRUE( *m_fastForwardParticles	!= nullptr , "Spaceship: missing particles reference" );
		REQUIRE_TRUE( *m_slowForwardParticles	!= nullptr , "Spaceship: missing particles reference" );
		REQUIRE_TRUE( *m_reverseParticles		!= nullptr , "Spaceship: missing particles reference" );
		REQUIRE_TRUE( *m_leftParticles			!= nullptr , "Spaceship: missing particles reference" );
		REQUIRE_TRUE( *m_rightParticles			!= nullptr , "Spaceship: missing particles reference" );

		 if( *m_fastForwardParticles	!= nullptr ) { m_fastForwardParticles->SetEnabled( false ); }
		 if( *m_slowForwardParticles	!= nullptr ) { m_slowForwardParticles->SetEnabled( false ); }
		 if( *m_reverseParticles		!= nullptr ) { m_reverseParticles	  ->SetEnabled( false ); }
		 if( *m_leftParticles			!= nullptr ) { m_leftParticles		  ->SetEnabled( false ); }
		 if( *m_rightParticles			!= nullptr ) { m_rightParticles	  ->SetEnabled( false ); }
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Update( const float _delta )
	{
		Transform * transform = m_gameobject->GetComponent<Transform>();

		// Lateral movement
		const btVector3 direction = m_input->GetInputDirection();
		const bool  stop = m_input->GetInputStop();
		const float leftForce   = _delta * m_lateralForce * m_input->GetInputLeft();
		const float forwardAxis = stop ? 0.f : _delta * m_input->GetInputForward();
		const float boost = m_input->GetInputBoost();	

		// Orientation
		if( ! direction.isZero() ) 
		{
			transform->LookAt( transform->GetPosition() + direction, btVector3::Up() );
		}

		// constrain position
		btVector3 pos = transform->GetPosition();
		pos.setY( 0.f );
		transform->SetPosition( pos );

		SpeedMode speedMode = forwardAxis < 0 ? SpeedMode::REVERSE : ( boost > 0 ? SpeedMode::FAST : ( boost < 0 ? SpeedMode::SLOW : SpeedMode::NORMAL ) );

		// Consume energy
		float totalConsumption = m_energyConsumedPerUnitOfForce * ( std::abs( leftForce ) + std::abs( m_forwardForces[speedMode] * forwardAxis ) );
		if ( !m_energy->TryRemoveEnergy( totalConsumption ) ) // not enough energy = go to slow speed mode
		{
			m_energy->TryRemoveEnergy( m_energy->GetEnergy() );
			if ( speedMode != SpeedMode::REVERSE )
			{
				speedMode = SpeedMode::SLOW;
			}
		}

		// Enable particle systems
		m_fastForwardParticles->SetEnabled( false );
		m_slowForwardParticles->SetEnabled( false );
		m_reverseParticles->SetEnabled( false );
		m_leftParticles->SetEnabled( false );
		m_rightParticles->SetEnabled( false );

		if ( forwardAxis != 0.f )
		{
			if ( speedMode == SpeedMode::SLOW || speedMode == SpeedMode::NORMAL ) { m_slowForwardParticles->SetEnabled( true ); }
			else if ( speedMode == SpeedMode::FAST ) { m_fastForwardParticles->SetEnabled( true ); }
			else if ( speedMode == SpeedMode::REVERSE ) { m_reverseParticles->SetEnabled( true ); }
		}
		if ( leftForce > 0.f ) { m_leftParticles->SetEnabled( true ); }
		else if ( leftForce < 0.f ) { m_rightParticles->SetEnabled( true ); }

		// Forces application		
		m_rigidbody->ApplyCentralForce( leftForce * transform->Left() );
		m_rigidbody->ApplyCentralForce( m_forwardForces[speedMode] * forwardAxis * transform->Forward() );
		m_rigidbody->GetBtBody()->setAngularVelocity(btVector3::Zero());

		// Drag
		btVector3 newVelocity = ( totalConsumption > 0.f ? m_activeDrag : m_passiveDrag ) * m_rigidbody->GetVelocity();
		newVelocity.setY( 0.f );
		m_rigidbody->SetVelocity( newVelocity );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::OnGui()
	{
		Actor::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "reverse force", &m_forwardForces[SpeedMode::REVERSE], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "slow forward force", &m_forwardForces[SpeedMode::SLOW], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "normal forward force", &m_forwardForces[SpeedMode::NORMAL], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "fast forward force", &m_forwardForces[SpeedMode::FAST], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "lateral force", &m_lateralForce, 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "active drag", &m_activeDrag, 0.001f, 0.f, 1.f );
			ImGui::DragFloat( "passive drag", &m_passiveDrag, 0.001f, 0.f, 1.f );
			ImGui::DragFloat( "energyConsumedPerUnitOfForce", &m_energyConsumedPerUnitOfForce, 0.0001f, 0.f, 1.f );
			ImGui::FanComponent( "fast forward particles", &m_fastForwardParticles );
			ImGui::FanComponent( "slow forward particles", &m_slowForwardParticles );
			ImGui::FanComponent( "reverse particles", &m_reverseParticles );
			ImGui::FanComponent( "left particles", &m_leftParticles );
			ImGui::FanComponent( "right particles", &m_rightParticles );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Load( const Json & _json ) {
		Actor::Load(_json);

		LoadVec4(  _json , "forward_forces", m_forwardForces );
		LoadFloat( _json, "lateral_force", m_lateralForce );
		LoadFloat( _json, "active_drag", m_activeDrag );
		LoadFloat( _json, "passive_drag", m_passiveDrag );
		LoadFloat( _json, "energy_consumed_per_unit_of_force", m_energyConsumedPerUnitOfForce );
		LoadComponentPtr( _json, "fast_forward_particles", m_fastForwardParticles );
		LoadComponentPtr( _json, "slow_forward_particles", m_slowForwardParticles );
		LoadComponentPtr( _json, "reverse_particles", m_reverseParticles );
		LoadComponentPtr( _json, "left_particles", m_leftParticles );
		LoadComponentPtr( _json, "right_particles", m_rightParticles );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Save( Json & _json ) const {
		SaveVec4(  _json, "forward_forces", m_forwardForces );
		SaveFloat( _json, "lateral_force", m_lateralForce );
		SaveFloat( _json, "active_drag", m_activeDrag );
		SaveFloat( _json, "passive_drag", m_passiveDrag );
		SaveFloat( _json, "energy_consumed_per_unit_of_force", m_energyConsumedPerUnitOfForce );
		SaveComponentPtr( _json, "fast_forward_particles", m_fastForwardParticles );
		SaveComponentPtr( _json, "slow_forward_particles", m_slowForwardParticles );
		SaveComponentPtr( _json, "reverse_particles", m_reverseParticles );
		SaveComponentPtr( _json, "left_particles", m_leftParticles );
		SaveComponentPtr( _json, "right_particles", m_rightParticles );

		Actor::Save( _json );
		
		return true;
	}
}