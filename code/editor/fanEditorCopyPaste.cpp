#include "fanGlobalIncludes.h"
#include "editor/fanEditorCopyPaste.h"

#include "editor/fanEditorSelection.h"
#include "scene/fanGameobject.h"
#include "scene/fanSceneInstantiate.h"
#include "scene/fanPrefab.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EditorCopyPaste::EditorCopyPaste( EditorSelection& _selection ) : m_selection( _selection ) {}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::OnCopy()
	{
		if ( m_selection.GetSelectedGameobject() != nullptr )
		{

			Prefab prefab;
			prefab.LoadFromGameobject( m_selection.GetSelectedGameobject() );

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
			m_selection.GetSelectedScene().CreateGameobject( prefab, m_selection.GetSelectedGameobject() );
		}		
	}
}