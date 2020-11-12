#pragma once

#include "scene/components/fanBoxShape.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void BoxShape::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::CubeShape16;
		_info.mGroup      = EngineGroups::ScenePhysics;
		_info.onGui       = &BoxShape::OnGui;
		_info.mEditorPath = "/";
		_info.mName       = "box_shape";
	}

    //========================================================================================================
    //========================================================================================================
    void BoxShape::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        BoxShape& boxShape = static_cast<BoxShape&>( _component );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            btVector3 extent = boxShape.GetScaling();
            if( ImGui::DragFloat3( "scaling", &extent[0], 0.05f, 0.f ) )
            {
                boxShape.SetScaling( extent );
            }
        }
        ImGui::PopItemWidth();
    }
}