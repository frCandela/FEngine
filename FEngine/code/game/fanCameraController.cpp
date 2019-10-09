#include "fanGlobalIncludes.h"
#include "game/fanCameraController.h"

#include "game/fanSpaceShip.h"
#include "core/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT(CameraController);
	REGISTER_TYPE_INFO(CameraController)

		//================================================================================================================================
		//================================================================================================================================
		void CameraController::Start() {
		m_transform = GetGameobject()->GetComponent<Transform>();
		m_camera = GetGameobject()->GetComponent<Camera>();
		if (m_camera == nullptr) {
			Debug::Warning("Game manager has no camera attached");
		}

		SpaceShip * ss = GetGameobject()->GetScene()->FindComponentOfType<SpaceShip>();
		if (ss) {
			m_target = ss->GetGameobject()->GetComponent<Transform>();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::Update(const float /*_delta*/) {
		if (m_target != nullptr) {
			m_transform->SetPosition(m_target->GetPosition() + m_heightFromTarget * btVector3::Up());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::OnGui() {
		Actor::OnGui();
		ImGui::DragFloat("height from target", &m_heightFromTarget, 0.25f, 0.5f, 10.f);
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool CameraController::Load(std::istream& _in) {
		Actor::Load(_in);

		if (!ReadSegmentHeader(_in, "heightFromTarget:")) { return false; }
		if (!ReadFloat(_in, m_heightFromTarget)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool CameraController::Save(std::ostream& _out, const int _indentLevel) const {
		Actor::Save(_out, _indentLevel);
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "heightFromTarget: " << m_heightFromTarget << std::endl;
		return true;
	}
}