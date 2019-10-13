#include "fanGlobalIncludes.h"
#include "game/fanSpaceShip.h"

#include "core/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"

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

		// Go forward
		float forward = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_W)) {
			forward = 1.f;
		}
		else if (Keyboard::IsKeyDown(GLFW_KEY_S)) {
			forward = -1.f;
		}

		// Turn
		float leftRotation = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_D)) {
			leftRotation = -1.f;
		}
		else if (Keyboard::IsKeyDown(GLFW_KEY_A)) {
			leftRotation = 1.f;
		}

		// Translation
		Transform * transform = GetGameobject()->GetComponent<Transform>();
		if (forward != 0.f) {
			forward *= m_velocity * _delta;
			m_speed += _delta * forward * transform->Forward(); // increases velocity			
		} 
		transform->SetPosition( transform->GetPosition() + m_speed );
		const btVector3 drag = m_drag * m_speed;
		m_speed -= _delta * drag;

		// Rotation
		if (leftRotation != 0.f) {
			leftRotation *= m_rotationSpeed * _delta;
			btQuaternion quat = transform->GetRotationQuat();
			btQuaternion rotationQuat(btVector3::Up(), leftRotation);
			transform->SetRotationQuat(quat * rotationQuat);
		}
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