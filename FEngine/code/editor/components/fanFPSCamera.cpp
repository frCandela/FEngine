#include "fanGlobalIncludes.h"

#include "editor/components/fanFPSCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"
#include "core/fanSignal.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"
#include "core/input/fanMouse.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	REGISTER_TYPE_INFO(FPSCamera, TypeInfo::Flags::EDITOR_COMPONENT, "editor/")

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::OnAttach() {
		Actor::OnAttach();
		m_speed = 10.f;
		m_speedMultiplier = 3.f;
		m_xySensitivity = btVector2(0.005f, 0.005f);
		m_transform = m_gameobject->GetComponent<Transform>();
		m_camera = m_gameobject->GetComponent<Camera>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	FPSCamera::~FPSCamera() {

	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Start() {
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::OnDisable() {
		Mouse::LockCursor( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Update(const float _delta) {

		if (Mouse::GetButtonPressed(Mouse::button1)) {
			Mouse::LockCursor(true);
		}
		if (Mouse::GetButtonDown(Mouse::button1, true ) == false ) {
			Mouse::LockCursor(false);
		}

		btVector3 position = m_transform->GetPosition();

		float forwardAxis = Input::Get().Manager().GetAxis( "editor_forward" );
		float upAxis =		Input::Get().Manager().GetAxis( "editor_up" );
		float leftAxis =	Input::Get().Manager().GetAxis( "editor_left" );
		float boost =		Input::Get().Manager().GetAxis( "editor_boost" );
		
		// Calculates speed
		float realSpeed = m_speed;
		if ( boost > 0.f ) {
			realSpeed *= m_speedMultiplier;
		}
		
		position += _delta * realSpeed * leftAxis * m_transform->Left();		// Camera goes left		
		position += _delta * realSpeed * upAxis * m_transform->Up();			// Camera goes up		
		position += _delta * realSpeed * forwardAxis * m_transform->Forward();	// Camera goes forward

		// Camera rotation
		const btVector2 mouseDelta = Mouse::GetDelta();
		const btVector2 mousePos = Mouse::GetPosition();
		if (Mouse::GetButtonDown(Mouse::button1)) {
			// Rotation depending on mouse movement
			const float invertAxis = -1;
			const btQuaternion rotationY(btVector3::Up(), -m_xySensitivity.x() * mouseDelta.x());
			const btQuaternion rotationX(invertAxis *m_transform->Left(), -m_xySensitivity.y() *mouseDelta.y());
			m_transform->SetRotationQuat(rotationX* rotationY* m_transform->GetRotationQuat());

			// Remove roll
			const btVector3 relativeLeft = m_transform->Left();
			const btVector3 leftNoRoll(relativeLeft.x(), 0, relativeLeft.z());
			const btVector3 axis = relativeLeft.cross(leftNoRoll);
			const float angle = leftNoRoll.angle(relativeLeft);
			if (angle != 0) {

				const btQuaternion rot(axis, angle);

				m_transform->SetRotationQuat(rot * m_transform->GetRotationQuat());
			}
		}
		m_transform->SetPosition(position);

		const Ray ray = m_camera->ScreenPosToRay(btVector2(0.9f, 0.9f));
		const float size = 0.002f;
		btVector3 offset = ray.origin + 0.1f*ray.direction;

		Debug::Render().DebugLine(offset, offset + btVector3(size, 0, 0), Color(1, 0, 0, 1));
		Debug::Render().DebugLine(offset, offset + btVector3(0, size, 0), Color(0, 1, 0, 1));
		Debug::Render().DebugLine(offset, offset + btVector3(0, 0, size), Color(0, 0, 1, 1));
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::OnGui() {
		Actor::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 ); {
			// SetSensitivity
			if (ImGui::Button("##SetSensitivity")) {
				SetXYSensitivity(btVector2(0.005f, 0.005f));
			} ImGui::SameLine();
			btVector2 xySensitivity = GetXYSensitivity();
			if (ImGui::DragFloat2("XY sensitivity", &xySensitivity[0], 1.f)) {
				SetXYSensitivity(xySensitivity);
			}

			// SetSpeed
			if (ImGui::Button("##SetSpeed")) {
				SetSpeed(10.f);
			} ImGui::SameLine();
			float speed = GetSpeed();
			if (ImGui::DragFloat("speed", &speed, 1.f)) {
				SetSpeed(speed);
			}

			// SetSpeedMultiplier
			if (ImGui::Button("##SetSpeedMultiplier")) {
				SetSpeedMultiplier(3.f);
			} ImGui::SameLine();
			float speedMultiplier = GetSpeedMultiplier();
			if (ImGui::DragFloat("speed multiplier", &speedMultiplier, 1.f)) {
				SetSpeedMultiplier(speedMultiplier);
			}
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FPSCamera::Load( const Json & _json ) {
		Actor::Load(_json);

		LoadVec2( _json, "xySensitivity", m_xySensitivity );
		LoadFloat( _json, "speed", m_speed );
		LoadFloat( _json, "speedMultiplier", m_speedMultiplier );
 		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FPSCamera::Save( Json & _json ) const {
		SaveVec2( _json, "xySensitivity", m_xySensitivity );
		SaveFloat(_json, "speed", m_speed );
		SaveFloat(_json, "speedMultiplier", m_speedMultiplier );
		Actor::Save( _json );
		return true;
	}
}