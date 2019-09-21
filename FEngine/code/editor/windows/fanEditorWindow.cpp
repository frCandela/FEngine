#include "fanGlobalIncludes.h"

#include "editor/windows/fanEditorWindow.h"
#include "core/files/fanSerializedValues.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		Window::Window( const std::string _name ) :
			m_name( _name ),
			m_isVisible(true) {

			std::stringstream ss;
			ss << "show_window_" << _name;
			m_jsonShowWindowKey = ss.str();

			bool value;
			if (SerializedValues::Get().GetValue(m_jsonShowWindowKey, value) == true) {
				SetVisible(value);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		Window::~Window() {
			SerializedValues::Get().SetValue(m_jsonShowWindowKey, m_isVisible);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Window::Draw() {
			if ( m_isVisible == true ) {
				ImGui::Begin(m_name.c_str(), &m_isVisible); {
					OnGui();
				} ImGui::End();
				SetVisible(m_isVisible);
			}		
		}
	}
}
