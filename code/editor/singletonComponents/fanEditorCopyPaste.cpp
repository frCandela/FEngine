#include "editor/singletonComponents/fanEditorCopyPaste.hpp"

#include <sstream>
#include "ecs/fanEcsWorld.hpp"
#include "editor/singletonComponents/fanEditorSelection.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletonComponents/fanScene.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::SetInfo( EcsSingletonInfo& _info )
	{
		_info.name = "editor copy/paste";
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		EditorCopyPaste& editorCopyPaste = static_cast<EditorCopyPaste&>( _component );

		editorCopyPaste.m_selection = &_world.GetSingleton<EditorSelection>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCopyPaste::OnCopy()
	{
		if ( m_selection->GetSelectedSceneNode() != nullptr )
		{
			Prefab prefab;
			prefab.CreateFromSceneNode( *m_selection->GetSelectedSceneNode() );

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
			prefab.Instanciate( *m_selection->GetSelectedSceneNode() );
		}
	}
} 