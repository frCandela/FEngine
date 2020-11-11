#prama once

#include "core/ecs/fanSlot.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ScenePointers::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Pointer16;
		_info.mGroup = EngineGroups::Scene;
		_info.onGui  = &ScenePointers::OnGui;
		_info.mName  = "scene pointers";
	}

    //========================================================================================================
    //========================================================================================================
    void ScenePointers::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
        ImGui::Text( "unresolved component pointers: %d", scenePointers.mUnresolvedComponentPtr.size() );
    }
}