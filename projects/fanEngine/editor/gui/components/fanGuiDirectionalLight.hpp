#pragma once

#include "engine/components/fanDirectionalLight.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiDirectionalLight
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::DirLight16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiDirectionalLight::OnGui;
            info.mEditorPath = "lights/";
            info.mEditorName = "Directional light";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            DirectionalLight& light = static_cast<DirectionalLight&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                // Filter color
                if( ImGui::Button( "##ambiant" ) ){ light.mAmbiant = Color::sBlack; }
                ImGui::SameLine();
                ImGui::ColorEdit3( "ambiant", light.mAmbiant.Data(), ImGui::fanColorEditFlags );

                if( ImGui::Button( "##diffuse" ) ){ light.mDiffuse = Color::sBlack; }
                ImGui::SameLine();
                ImGui::ColorEdit3( "diffuse", light.mDiffuse.Data(), ImGui::fanColorEditFlags );

                if( ImGui::Button( "##specular" ) ){ light.mSpecular = Color::sBlack; }
                ImGui::SameLine();
                ImGui::ColorEdit3( "specular", light.mSpecular.Data(), ImGui::fanColorEditFlags );


// 			const Transform& transform = m_gameobject->GetTransform();
// 			const Vector3 pos = transform.GetPosition();
// 			const Vector3 dir = transform.Forward();
// 			const Vector3 up = transform.Up();
// 			const Vector3 left = transform.Left();
// 			const float length = 2.f;
// 			const float radius = 0.5f;
// 			const Color color = Color::Yellow;
// 			Vector3 offsets[5] = { Vector3_Zero, radius * up ,-radius * up, radius * left ,-radius * left };
// 			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
// 			{
// 				const Vector3 offset = offsets[offsetIndex];
// 				_world.GetSingleton<RenderDebug>().DebugLine( pos + offset, pos + offset + length * dir, color );
// 			}
// 			_world.GetSingleton<RenderDebug>().DebugSphere( transform.GetBtTransform(), radius, 0, color );
            }
            ImGui::PopItemWidth();
        }
    };
}