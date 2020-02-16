#include "editor/fanEditorCopyPaste.hpp"
#include "editor/fanEditorSelection.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanPrefab.hpp"

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
			prefab.CreateFromGameobject( m_selection.GetSelectedGameobject() );

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
		if ( prefab.CreateFromJson( pastedJson ) )
		{
			m_selection.GetSelectedScene().CreateGameobject( prefab, m_selection.GetSelectedGameobject() );
		}
	}
}