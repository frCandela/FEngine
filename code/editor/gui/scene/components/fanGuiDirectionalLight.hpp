#prama once

#include "scene/components/fanDirectionalLight.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::DirLight16;
		_info.mGroup      = EngineGroups::SceneRender;
		_info.onGui       = &DirectionalLight::OnGui;
		_info.mEditorPath = "lights/";
		_info.mName       = "directional light";
	}

    //========================================================================================================
    //========================================================================================================
    void DirectionalLight::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        DirectionalLight& light = static_cast<DirectionalLight&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            // Filter color
            if( ImGui::Button( "##ambiant" ) ) { light.mAmbiant = Color::sBlack; } ImGui::SameLine();
            ImGui::ColorEdit3( "ambiant", light.mAmbiant.Data(), ImGui::fanColorEditFlags );

            if( ImGui::Button( "##diffuse" ) ) { light.mDiffuse = Color::sBlack; } ImGui::SameLine();
            ImGui::ColorEdit3( "diffuse", light.mDiffuse.Data(), ImGui::fanColorEditFlags );

            if( ImGui::Button( "##specular" ) ) { light.mSpecular = Color::sBlack; } ImGui::SameLine();
            ImGui::ColorEdit3( "specular", light.mSpecular.Data(), ImGui::fanColorEditFlags );


// 			const Transform& transform = m_gameobject->GetTransform();
// 			const btVector3 pos = transform.GetPosition();
// 			const btVector3 dir = transform.Forward();
// 			const btVector3 up = transform.Up();
// 			const btVector3 left = transform.Left();
// 			const float length = 2.f;
// 			const float radius = 0.5f;
// 			const Color color = Color::Yellow;
// 			btVector3 offsets[5] = { btVector3::Zero(), radius * up ,-radius * up, radius * left ,-radius * left };
// 			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
// 			{
// 				const btVector3 offset = offsets[offsetIndex];
// 				_world.GetSingleton<RenderDebug>().DebugLine( pos + offset, pos + offset + length * dir, color );
// 			}
// 			_world.GetSingleton<RenderDebug>().DebugSphere( transform.GetBtTransform(), radius, 0, color );
        } ImGui::PopItemWidth();
    }
}