#include "fanGlobalIncludes.h"
#include "editor/fanEditorCopyPaste.h"

#include "fanEngine.h"
#include "scene/fanGameobject.h"
#include "scene/fanSceneInstantiate.h"
#include "scene/fanPrefab.h"

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

			Prefab prefab;
			prefab.LoadFromGameobject( m_engine.GetSelectedGameobject() );

			std::stringstream ss;
			ss << prefab.GetJson();

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

		Prefab prefab;
		if ( prefab.LoadFromJson( pastedJson ) )
		{
			m_engine.GetScene().CreateGameobject( prefab, m_engine.GetSelectedGameobject() );
		}		
	}
}