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

		btVector2 mouseDelta = Mouse::GetDelta();
		btVector2 mousePos = Mouse::GetPosition();

		if (Mouse::IsKeyDown(Mouse::button1)) {
			const btQuaternion rotationY(Transform::worldUp, m_xySensitivity.x() * mouseDelta.x() );
			const btQuaternion rotationX(m_transform.Right(), m_xySensitivity.y() *mouseDelta.y() );
			m_transform.SetRotationQuat(rotationY * rotationX * m_transform.GetRotationQuat());
		}
		m_transform.SetPosition(position);
	}

	//================================================================================================================================
	//================================================================================================================================
	void FPSCamera::Stop() {
		std::cout << "Stop" << std::endl;
	}
}