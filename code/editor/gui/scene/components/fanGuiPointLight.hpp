#prama once

#include "scene/components/fanPointLight.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PointLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::PointLight16;
		_info.mGroup      = EngineGroups::SceneRender;
		_info.onGui       = &PointLight::OnGui;
		_info.mEditorPath = "lights/";
		_info.mName       = "point light";
	}

    //========================================================================================================
    //========================================================================================================
    void PointLight::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        PointLight& pointLight = static_cast<PointLight&>( _component );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
        {

            // Filter color
            if( ImGui::Button( "##ambiantcpl2" ) ) { pointLight.mAmbiant = Color( 0.0f ); } ImGui::SameLine();
            ImGui::ColorEdit3( "ambiant##ambiantpl2", pointLight.mAmbiant.Data(), ImGui::fanColorEditFlags );
            if( ImGui::Button( "##diffusecpl2" ) ) { pointLight.mDiffuse = Color::sWhite; } ImGui::SameLine();
            ImGui::ColorEdit3( "diffuse##diffusepl2", pointLight.mDiffuse.Data(), ImGui::fanColorEditFlags );
            if( ImGui::Button( "##specularcpl2" ) ) { pointLight.mSpecular = Color::sWhite ; } ImGui::SameLine();
            ImGui::ColorEdit3( "specular##specularpl2", pointLight.mSpecular.Data(), ImGui::fanColorEditFlags );
            // Attenuation

            ImGui::Text( "attenuation :" );
            ImGui::SameLine();
            ImGui::FanShowHelpMarker(
                    "Light intensity fades out with distance \n"
                    "Fading follows the following formula : \n"
                    "constant + linear * d + quadratic * d*d  \n"
                    "(d=distance)" );
            if( ImGui::Button( "##constant attenuation" ) )
            {
                pointLight.mAttenuation[ Attenuation::Constant] = 0.f;
            }
            ImGui::SameLine();
            ImGui::DragFloat( "constant",
                              &pointLight.mAttenuation[Attenuation::Constant],
                              0.01f,
                              0.f,
                              100.f );
            if( ImGui::Button( "##linear attenuation" ) )
            {
                pointLight.mAttenuation[Attenuation::Linear] = 0.f;
            }
            ImGui::SameLine();
            ImGui::DragFloat( "linear", &pointLight.mAttenuation[Attenuation::Linear], 0.001f, 0.f, 100.f );
            if( ImGui::Button( "##quadratic attenuation" ) )
            {
                pointLight.mAttenuation[Attenuation::Quadratic] = 0.f;
            }
            ImGui::SameLine();
            ImGui::DragFloat( "quadratic",
                              &pointLight.mAttenuation[Attenuation::Quadratic],
                              0.0001f,
                              0.f,
                              100.f );

        } ImGui::PopItemWidth();
    }
}