#include "fanIncludes.h"

#include "editor/components/fanFPSCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "util/fanSignal.h"
#include "util/fanInput.h"
#include "fanEngine.h"
#include "vulkan/vkRenderer.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	FPSCamera::FPSCamera(Gameobject * _gameobject) :
		Actor(_gameobject)
		, m_transform(*GetGameobject()->GetComponent<scene::Transform>())
		, m_speed( 10.f)
		, m_speedMultiplier( 3.f)
		, m_xySensitivity ( btVector2(0.005f, 0.005f) ){

	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Start() {
		std::cout << "Start" << std::endl;		
		
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Update(const float _delta) {

		if (Mouse::IsButtonPressed(Mouse::button1)) {
			Mouse::LockCursor(true);
		}
		if (Mouse::IsButtonReleased( Mouse::button1)) {
			Mouse::LockCursor(false);
		}

		btVector3 position = m_transform.GetPosition();
		
		// Calculates speed
		float realSpeed = m_speed;
		if (Keyboard::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			realSpeed *= m_speedMultiplier;
		}

		// Camera goes right
		float rightAxis = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_D))
			rightAxis += 1.f;
		else if (Keyboard::IsKeyDown(GLFW_KEY_A))
			rightAxis -= 1.f;
		position += _delta * realSpeed * rightAxis * m_transform.Right();


		// Camera goes up
		float upAxis = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_E))
			upAxis += 1.f;
		else if (Keyboard::IsKeyDown(GLFW_KEY_Q))
			upAxis -= 1.f;
		position += _delta * realSpeed * upAxis * m_transform.Up();

		// Camera goes forward
		float forwardAxis = 0.f;
		if (Keyboard::IsKeyDown(GLFW_KEY_W))
			forwardAxis += 1.f;
		else if (Keyboard::IsKeyDown(GLFW_KEY_S))
			forwardAxis -= 1.f;
		position += _delta * realSpeed * forwardAxis * m_transform.Forward();

		// Camera rotation
		const btVector2 mouseDelta = Mouse::GetDelta();
		const btVector2 mousePos = Mouse::GetPosition();
		if (Mouse::IsKeyDown(Mouse::button1)) {
			// Rotation depending on mouse movement
			const btQuaternion rotationY(btVector3::Up(), -m_xySensitivity.x() * mouseDelta.x() );
			const btQuaternion rotationX(m_transform.Right(), -m_xySensitivity.y() *mouseDelta.y() );
			m_transform.SetRotationQuat( rotationX*rotationY* m_transform.GetRotationQuat());

			// Remove roll
			const btVector3 relativeRight = m_transform.Right();
			const btVector3 rightNoRoll(relativeRight.x(),0, relativeRight.z());
			const btVector3 axis = relativeRight.cross(rightNoRoll);
			const float angle = rightNoRoll.angle(relativeRight);
			if (angle != 0) {

				const btQuaternion rot(axis, angle);

				m_transform.SetRotationQuat(rot * m_transform.GetRotationQuat());
			}
		}
		m_transform.SetPosition(position);

		const float size = 0.1f;
		btVector3 offset = m_transform.GetPosition() + m_transform.Forward();

		fan::Engine::GetEngine().GetRenderer().DebugLine(offset, offset + btVector3(size, 0, 0), vk::Color(1, 0, 0, 1));
		fan::Engine::GetEngine().GetRenderer().DebugLine(offset, offset + btVector3(0, size, 0), vk::Color(0, 1, 0, 1));
		fan::Engine::GetEngine().GetRenderer().DebugLine(offset, offset + btVector3(0, 0, size), vk::Color(0, 0, 1, 1));
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Stop() {
		std::cout << "Stop" << std::endl;
	}
}