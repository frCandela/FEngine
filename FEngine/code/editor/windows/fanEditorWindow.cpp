#include "fanGlobalIncludes.h"

#include "editor/windows/fanEditorWindow.h"
#include "core/files/fanSerializedValues.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EditorWindow::EditorWindow(const std::string _name, const ImGui::IconType _iconType ) :
		m_name(_name)
		,m_isVisible(true)
		,m_iconType(_iconType)
	{

		std::stringstream ss;
		ss << "show_window_" << _name;
		m_jsonShowWindowKey = ss.str();

		bool value = false;
		if ( SerializedValues::Get().GetBool(m_jsonShowWindowKey.c_str(), value) ) {
			SetVisible(value);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EditorWindow::~EditorWindow() {
		SerializedValues::Get().SetBool(m_jsonShowWindowKey.c_str(), m_isVisible);
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorWindow::Draw() {
		if (m_isVisible == true) {
			ImGui::Begin(m_name.c_str(), &m_isVisible); {
				OnGui();
			} ImGui::End();
			SetVisible(m_isVisible);
		}
	}
}
