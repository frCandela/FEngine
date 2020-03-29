#include "editor/fanEditorCopyPaste.hpp"
#include "editor/fanEditorSelection.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletonComponents/fanScene.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EditorCopyPaste::EditorCopyPaste( EditorSelection& _selection ) : m_selection( _selection ) {}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::OnCopy()
	{
		if ( m_selection.GetSelectedSceneNode() != nullptr )
		{
			Prefab prefab;
			prefab.CreateFromSceneNode( *m_selection.GetSelectedSceneNode() );

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
			prefab.Instanciate( *m_selection.GetSelectedSceneNode() );
		}
	}
} 