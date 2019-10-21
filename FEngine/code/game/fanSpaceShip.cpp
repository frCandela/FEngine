#include "fanGlobalIncludes.h"
#include "game/fanSpaceShip.h"

#include "core/fanTime.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
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
		m_speed = btVector3(0.f,0.f,0.f);
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

		// Set rotation TODO: create a proper LookAt method in the Transform component
		float angle = btDegrees( btVector3::Forward().angle(mouseDir) );
		float sign = mouseDir.dot( btVector3::Left() ) > 0.f ? 1.f : -1.f;
		transform->SetRotationEuler( btVector3(0.f, sign * angle,0.f) );

		// Go forward
		float forward	= Input::Get().Manager().GetAxis( "game_forward");
		//float left		= Input::Get().Manager().GetAxis( "game_left" );

		// Translation
		
		if (forward != 0.f) {
			forward *= m_velocity * _delta;
			m_speed += _delta * forward * transform->Forward(); // increases velocity			
		} 
		transform->SetPosition( transform->GetPosition() + m_speed );
		const btVector3 drag = m_drag * m_speed;
		m_speed -= _delta * drag;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::OnGui() {
		Actor::OnGui();

		ImGui::DragFloat3("speed", &m_speed[0], 0.01f, 0.f, 10.f );
		ImGui::DragFloat("velocity", &m_velocity, 0.01f, 0.f, 10.f);
		ImGui::DragFloat( "drag", &m_drag, 0.01f, 0.f, 1.f );
		ImGui::DragFloat("rotation_speed", &m_rotationSpeed, 0.01f, 0.f, 10.f);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Load( Json & _json ) {
		Actor::Load(_json);

		LoadFloat( _json, "speed", m_velocity );
		LoadFloat( _json, "rotation_speed", m_rotationSpeed );
		LoadFloat( _json, "drag", m_drag );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShip::Save( Json & _json ) const {
		SaveFloat( _json, "speed", m_velocity );
		SaveFloat( _json, "rotation_speed", m_rotationSpeed );
		SaveFloat( _json, "drag", m_drag );
		Actor::Save( _json );
		
		return true;
	}
}