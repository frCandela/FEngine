#include "fanGlobalIncludes.h"

#include "game/fanPlanet.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"

namespace fan {
	namespace game {

		bool Planet::ms_registered = Component::RegisterComponent(new Planet());

		REGISTER_TYPE_INFO(Planet)
		
		//================================================================================================================================
		//================================================================================================================================
		void Planet::Start() {
			Debug::Log("Start planet");
		}

		//================================================================================================================================
		//================================================================================================================================
		void Planet::Update(const float _delta) {
			scene::Transform * transform = GetEntity()->GetComponent<scene::Transform>();

			btVector3 pos = transform->GetPosition();
			if (pos.getY() > 3 ) {
				m_dir = -1.f;
			}
			else if (pos.getY() < 0 ) {
				m_dir = 1.f;
			}
			pos[1] += 5.f*m_dir*_delta;

			transform->SetPosition(pos);

			ImGui::Begin("planet window !"); {
				if ( ImGui::Button("Button") ) {

				}
			} ImGui::End();
		}
	}
}