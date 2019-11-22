#include "fanGlobalIncludes.h"
#include "editor/fanEditorCopyPaste.h"

#include "fanEngine.h"
#include "scene/fanGameobject.h"
#include "scene/fanSceneInstantiate.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EditorCopyPaste::EditorCopyPaste( Engine& _engine ) : m_engine( _engine ) {}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::OnCopy()
	{
		if ( m_engine.GetSelectedGameobject() != nullptr )
		{
			Json objectJson;
			m_engine.GetSelectedGameobject()->Save( objectJson );

			std::stringstream ss;
			ss << objectJson;

			ImGui::SetClipboardText( ss.str().c_str() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::OnPaste()
	{
		std::stringstream ss;
		ss << ImGui::GetClipboardText();
		Json pastedJson;
		ss >> pastedJson;

		m_engine.GetScene().Instantiate().InstantiateJson( pastedJson, m_engine.GetSelectedGameobject() );
	}
}