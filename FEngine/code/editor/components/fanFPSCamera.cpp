#include "fanGlobalIncludes.h"

#include "editor/components/fanFPSCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"
#include "scene/components/fanCamera.h"
#include "core/fanSignal.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "fanEngine.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT(FPSCamera);
	REGISTER_TYPE_INFO(FPSCamera)

		//================================================================================================================================
		//================================================================================================================================
		void FPSCamera::OnAttach() {
		Actor::OnAttach();
		m_speed = 10.f;
		m_speedMultiplier = 3.f;
		m_xySensitivity = btVector2(0.005f, 0.005f);
		m_transform = GetEntity()->GetComponent<Transform>();
		m_camera = GetEntity()->GetComponent<Camera>();
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
		Debug::Log("Start");
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Update(const float _delta) {

		if (Mouse::GetButtonPressed(Mouse::button1)) {
			Mouse::LockCursor(true);
		}
		if (Mouse::GetButtonDown(Mouse::button1) == false ) {
			Mouse::LockCursor(false);
		}

		btVector3 position = m_transform->GetPosition();

		// Calculates speed
		float realSpeed = m_speed;
		if (Keyboard::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			realSpeed *= m_speedMultiplier;
		}

		// Camera goes left
		float rightAxis = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_D)) {
			rightAxis -= 1.f;
		}
		else if (Keyboard::IsKeyDown(GLFW_KEY_A)) {
			rightAxis += 1.f;
		}
		position += _delta * realSpeed * rightAxis * m_transform->Left();


		// Camera goes up
		float upAxis = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_E)) {
			upAxis += 1.f;
		}
		else if (Keyboard::IsKeyDown(GLFW_KEY_Q)) {
			upAxis -= 1.f;
		}
		position += _delta * realSpeed * upAxis * m_transform->Up();

		// Camera goes forward
		float forwardAxis = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_W)) {
			forwardAxis += 1.f;
		}
		else if (Keyboard::IsKeyDown(GLFW_KEY_S)) {
			forwardAxis -= 1.f;
		}
		position += _delta * realSpeed * forwardAxis * m_transform->Forward();

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
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FPSCamera::Load(std::istream& _in) {
		Actor::Load(_in);

		if (!ReadSegmentHeader(_in, "xySensitivity:")) { return false; }
		if (!ReadFloat(_in, m_xySensitivity[0])) { return false; }
		if (!ReadFloat(_in, m_xySensitivity[1])) { return false; }

		if (!ReadSegmentHeader(_in, "speed:")) { return false; }
		if (!ReadFloat(_in, m_speed)) { return false; }

		if (!ReadSegmentHeader(_in, "speedMultiplier:")) { return false; }
		if (!ReadFloat(_in, m_speedMultiplier)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FPSCamera::Save(std::ostream& _out, const int _indentLevel) const {
		Actor::Save(_out, _indentLevel);
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "xySensitivity:   " << m_xySensitivity[0] << " " << m_xySensitivity[1] << std::endl;
		_out << indentation << "speed:           " << m_speed << std::endl;
		_out << indentation << "speedMultiplier: " << m_speedMultiplier << std::endl;
		return true;
	}
}