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

namespace fan {
	REGISTER_EDITOR_COMPONENT(SpaceShip)
	REGISTER_TYPE_INFO(SpaceShip)

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Start() {
		m_rigidbody = GetGameobject()->GetComponent<Rigidbody>();
		m_particleSystem = GetGameobject()->GetComponent<ParticleSystem>();
		m_particleSystem->SetEnabled(false);

		if ( m_rigidbody == nullptr ) {
			Debug::Warning() << "SpaceShip: no rigidBody found on " << GetGameobject()->GetName() << Debug::Endl();
		} else {
			m_rigidbody->GetBtBody()->setSleepingThresholds(0.f,0.f);
		}
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
		float left		= Input::Get().Manager().GetAxis( "game_left" );
		m_rigidbody->ApplyCentralForce( _delta * m_lateralForce * left * transform->Left() );

		// Go forward
		float forward = Input::Get().Manager().GetAxis( "game_forward" );		
		m_rigidbody->ApplyCentralForce( _delta * m_forwardForce * forward * transform->Forward() );

		// Drag
		btVector3 newVelocity = m_drag * m_rigidbody->GetVelocity();
		newVelocity.setY(0.f);
		m_rigidbody->SetVelocity( newVelocity );

		// Particles
		m_particleSystem->SetEnabled( forward > 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::OnGui() {
		Actor::OnGui();

		ImGui::DragFloat( "forward force", &m_forwardForce, 1.f, 0.f, 100000.f);
		ImGui::DragFloat( "lateral force", &m_lateralForce, 1.f, 0.f, 100000.f );
		ImGui::DragFloat( "drag", &m_drag, 0.01f, 0.f, 1.f );

	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Load( Json & _json ) {
		Actor::Load(_json);

		LoadFloat( _json, "forward_force", m_forwardForce );
		LoadFloat( _json, "lateral_force", m_lateralForce );
		LoadFloat( _json, "drag", m_drag );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Save( Json & _json ) const {
		SaveFloat( _json, "forward_force", m_forwardForce );
		SaveFloat( _json, "lateral_force", m_lateralForce );
		SaveFloat( _json, "drag", m_drag );
		Actor::Save( _json );
		
		return true;
	}
}