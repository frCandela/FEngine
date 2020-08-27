#include "editor/singletons/fanEditorCopyPaste.hpp"

#include <sstream>
#include "ecs/fanEcsWorld.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletons/fanScene.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorCopyPaste::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mName  = "editor copy/paste";
		_info.mIcon  = ImGui::CopyPaste16;
		_info.mGroup = EngineGroups::Editor;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorCopyPaste::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		EditorCopyPaste& editorCopyPaste = static_cast<EditorCopyPaste&>( _component );

		editorCopyPaste.mSelection = &_world.GetSingleton<EditorSelection>();
	}

	//========================================================================================================
	//========================================================================================================
	void EditorCopyPaste::OnCopy()
	{
		if ( mSelection->GetSelectedSceneNode() != nullptr )
		{
			Prefab prefab;
			prefab.CreateFromSceneNode( *mSelection->GetSelectedSceneNode() );

			std::stringstream ss;
			ss << prefab.mJson;

			ImGui::SetClipboardText( ss.str().c_str() );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void EditorCopyPaste::OnPaste()
	{
		std::stringstream ss;
		ss << ImGui::GetClipboardText();
		Json pastedJson;
		ss >> pastedJson;

		Prefab prefab;
		if ( prefab.CreateFromJson( pastedJson ) )
		{
            prefab.Instantiate( *mSelection->GetSelectedSceneNode() );
		}
	}
} 