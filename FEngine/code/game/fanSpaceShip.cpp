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
		m_rigidbody->ApplyCentralForce(  leftForce * transform->Left() );

		// Go forward
		const float boostDirection = Input::Get().Manager().GetAxis( "game_boost" );
		const float force = boostDirection > 0 ? m_highForwardForce : ( boostDirection < 0 ? m_lowForwardForce : m_forwardForce );
		const float forwardForce = _delta * force * Input::Get().Manager().GetAxis( "game_forward" );
		m_rigidbody->ApplyCentralForce(  forwardForce * transform->Forward() );

		// Drag
		btVector3 newVelocity = m_drag * m_rigidbody->GetVelocity();
		newVelocity.setY(0.f);
		m_rigidbody->SetVelocity( newVelocity );

		// Particles
		m_particleSystem->SetEnabled( forwardForce > 0 );

		// Energy consumption
		float totalConsumption = m_energyConsumedPerUnitOfForce * ( std::abs(leftForce) + std::abs(forwardForce) );
		m_energy->AddEnergy( -totalConsumption );
	}

	//===============================================z=================================================================================
	void SpaceShip::OnGui() {
		Actor::OnGui();

		ImGui::DragFloat( "low forward force", &m_lowForwardForce, 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "forward force", &m_forwardForce, 1.f, 0.f, 100000.f);
		ImGui::DragFloat( "high forward force", &m_highForwardForce, 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "lateral force", &m_lateralForce, 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "drag", &m_drag, 0.01f, 0.f, 1.f );
		ImGui::DragFloat( "energyConsumedPerUnitOfForce", &m_energyConsumedPerUnitOfForce, 0.001f, 0.f, 1.f );

	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Load( Json & _json ) {
		Actor::Load(_json);

		LoadFloat( _json, "low_forward_force", m_lowForwardForce );
		LoadFloat( _json, "forward_force", m_forwardForce );
		LoadFloat( _json, "high_forward_force", m_highForwardForce );
		LoadFloat( _json, "lateral_force", m_lateralForce );
		LoadFloat( _json, "drag", m_drag );
		LoadFloat( _json, "energy_consumed_per_unit_of_force", m_energyConsumedPerUnitOfForce );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Save( Json & _json ) const {
		SaveFloat( _json, "low_forward_force", m_lowForwardForce );
		SaveFloat( _json, "forward_force", m_forwardForce );
		SaveFloat( _json, "high_forward_force", m_highForwardForce );
		SaveFloat( _json, "lateral_force", m_lateralForce );
		SaveFloat( _json, "drag", m_drag );
		SaveFloat( _json, "energy_consumed_per_unit_of_force", m_energyConsumedPerUnitOfForce );
		Actor::Save( _json );
		
		return true;
	}
}