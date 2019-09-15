#include "fanGlobalIncludes.h"
#include "game/fanCameraController.h"

#include "fanEngine.h"
#include "game/fanSpaceShip.h"
#include "core/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanScene.h"
#include "scene/fanEntity.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "renderer/fanRenderer.h"


namespace fan {
	namespace game {

		REGISTER_EDITOR_COMPONENT(CameraController);
		REGISTER_TYPE_INFO(CameraController)
		
		//================================================================================================================================
		//================================================================================================================================
		void CameraController::Start() {
			m_transform = GetEntity()->GetComponent<scene::Transform>();
			m_camera = GetEntity()->GetComponent<scene::Camera>();
			if (m_camera == nullptr) {
				Debug::Warning("Game manager has no camera attached");
			}

			SpaceShip * ss = GetEntity()->GetScene()->FindComponentOfType<SpaceShip>();
			if (ss) {
				m_target = ss->GetEntity()->GetComponent<scene::Transform>();
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
			ImGui::DragFloat("height from target", &m_heightFromTarget, 0.25f, 0.5f , 10.f);
		}

		//================================================================================================================================
		//================================================================================================================================
		void CameraController::OnDetach() {
			Actor::OnDetach();
		}

		//================================================================================================================================
		//================================================================================================================================
		bool CameraController::Load(std::istream& _in) {
 			if (!ReadSegmentHeader(_in, "heightFromTarget:")) { return false; }
 			if (!ReadFloat(_in, m_heightFromTarget)) { return false; }
			return true; 
		}

		//================================================================================================================================
		//================================================================================================================================
		bool CameraController::Save(std::ostream& _out, const int _indentLevel) const {
 			const std::string indentation = GetIndentation(_indentLevel);
 			_out << indentation << "heightFromTarget: " << m_heightFromTarget << std::endl;
			return true; 
		}
	}
}