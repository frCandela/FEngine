#include "editor/singletons/fanEditorCopyPaste.hpp"

#include <sstream>
#include "imgui/imgui.h"
#include "core/ecs/fanEcsWorld.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "engine/fanPrefab.hpp"
#include "engine/singletons/fanScene.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorCopyPaste::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
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