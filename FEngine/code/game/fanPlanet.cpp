#include "fanGlobalIncludes.h"

#include "game/fanPlanet.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"

namespace fan {
	namespace game {

		bool Planet::m_registered = Component::RegisterComponent(new Planet());

		REGISTER_TYPE_INFO(Planet)
		//================================================================================================================================
		//================================================================================================================================
		void Planet::Initialize() {
		}
		
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
			pos[1] += 5.f*m_dir*(_delta < 0.1f ? _delta : 0.1f );
			if (pos.getY() > 3 || pos.getY() < 0) {
				m_dir = -m_dir;
			}
			transform->SetPosition(pos);

			ImGui::Begin("planet window !"); {
				if ( ImGui::Button("Button") ) {

				}
			} ImGui::End();
		}
	}
}