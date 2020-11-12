#pragma once

#include "scene/components/fanSphereShape.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SphereShape::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::SphereShape16;
		_info.mGroup      = EngineGroups::ScenePhysics;
		_info.onGui       = &SphereShape::OnGui;
		_info.mEditorPath = "/";
		_info.mName       = "sphere shape";
	}

    //========================================================================================================
    //========================================================================================================
    void SphereShape::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        SphereShape& sphereShape = static_cast<SphereShape&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            float radius = sphereShape.GetRadius();
            if( ImGui::DragFloat( "radius##sphshapradius", &radius, 0.1f, 0.f ) )
            {
                sphereShape.SetRadius( radius );
            }
        }
        ImGui::PopItemWidth();
    }
}