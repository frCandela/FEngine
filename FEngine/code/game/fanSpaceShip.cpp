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

namespace fan {
	REGISTER_EDITOR_COMPONENT(SpaceShip)
	REGISTER_TYPE_INFO(SpaceShip)

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Start() {
		REQUIRE_COMPONENT( WithEnergy, m_energy )
		REQUIRE_COMPONENT( Rigidbody, m_rigidbody )
		REQUIRE_COMPONENT( ParticleSystem, m_particleSystem )
			
		if( m_particleSystem ) { m_particleSystem->SetEnabled( false ); }		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Update(const float _delta) {

		// Get mouse world pos
		Camera * camera			= m_gameobject->GetScene()->GetMainCamera();
		btVector3 mouseWorldPos = camera->ScreenPosToRay( Mouse::GetScreenSpacePosition() ).origin;
		mouseWorldPos.setY(0);

		// Get mouse direction
		Transform * transform = m_gameobject->GetComponent<Transform>();
		btVector3 mouseDir = mouseWorldPos - m_gameobject->GetTransform()->GetPosition();
		mouseDir.normalize();

		// Orientation
		transform->LookAt( mouseWorldPos , btVector3::Up() );
		btVector3 pos = transform->GetPosition();
		pos.setY( 0.f );
		transform->SetPosition( pos );
		 
		// Lateral movement
		const float leftForce	= _delta * m_lateralForce * Input::Get().Manager().GetAxis( "game_left" );
		

		// Go forward
		const float forwardAxis = _delta * Input::Get().Manager().GetAxis( "game_forward" );
		const float boostDirection = Input::Get().Manager().GetAxis( "game_boost" );
		SpeedMode speedMode = forwardAxis < 0 ? SpeedMode::REVERSE : ( boostDirection > 0 ? SpeedMode::FAST : ( boostDirection < 0 ? SpeedMode::SLOW : SpeedMode::NORMAL ) );
		
		// Consume energy
		float totalConsumption = m_energyConsumedPerUnitOfForce * ( std::abs( leftForce ) + std::abs( m_forwardForces[speedMode] * forwardAxis ) );
		if ( !m_energy->TryRemoveEnergy( totalConsumption ) ) // not enought energy = go to slow speed mode
		{
			m_energy->TryRemoveEnergy( m_energy->GetEnergy() );
			if( speedMode != SpeedMode::REVERSE ) {
				speedMode = SpeedMode::SLOW;
			}
		}

		// Particles
		m_particleSystem->SetEnabled( forwardAxis > 0 );

		// Forces application		
		m_rigidbody->ApplyCentralForce( leftForce * transform->Left() );
		m_rigidbody->ApplyCentralForce( m_forwardForces[speedMode] * forwardAxis * transform->Forward() );
	

		// Drag
		btVector3 newVelocity = ( totalConsumption > 0.f ? m_activeDrag : m_passiveDrag ) * m_rigidbody->GetVelocity();
		newVelocity.setY( 0.f );
		m_rigidbody->SetVelocity( newVelocity );
	}

	//================================================================================================================================
	void SpaceShip::OnGui() {
		Actor::OnGui();

		ImGui::DragFloat( "reverse force", &m_forwardForces[SpeedMode::REVERSE], 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "slow forward force", &m_forwardForces[SpeedMode::SLOW], 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "normal forward force", &m_forwardForces[SpeedMode::NORMAL], 1.f, 0.f, 100000.f);
		ImGui::DragFloat( "fast forward force", &m_forwardForces[SpeedMode::FAST], 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "lateral force", &m_lateralForce, 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "active drag", &m_activeDrag, 0.001f, 0.f, 1.f );
		ImGui::DragFloat( "passive drag", &m_passiveDrag, 0.001f, 0.f, 1.f );
		ImGui::DragFloat( "energyConsumedPerUnitOfForce", &m_energyConsumedPerUnitOfForce, 0.0001f, 0.f, 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Load( Json & _json ) {
		Actor::Load(_json);

		LoadVec4(  _json , "forward_forces", m_forwardForces );
		LoadFloat( _json, "lateral_force", m_lateralForce );
		LoadFloat( _json, "active_drag", m_activeDrag );
		LoadFloat( _json, "passive_drag", m_passiveDrag );
		LoadFloat( _json, "energy_consumed_per_unit_of_force", m_energyConsumedPerUnitOfForce );

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
		Actor::Save( _json );
		
		return true;
	}
}