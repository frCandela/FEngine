#include "scene/components/fanPointLight.hpp"
#include "bullet/LinearMath/btVector3.h"
#include "core/fanSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PointLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::POINT_LIGHT16;
		_info.group = EngineGroups::SceneRender;
		_info.onGui = &PointLight::OnGui;
		_info.load = &PointLight::Load;
		_info.save = &PointLight::Save;
		_info.editorPath = "lights/";
		_info.name = "point light";
	}

	//========================================================================================================
	//========================================================================================================
	void PointLight::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
		pointLight.mAmbiant  = Color::White;
		pointLight.mDiffuse  = Color::White;
		pointLight.mSpecular = Color::White;
		pointLight.mAttenuation[0] = 1.0f;
		pointLight.mAttenuation[1] = 0.5f;
		pointLight.mAttenuation[2] = 0.2f;
	}

    //========================================================================================================
    //========================================================================================================
    float PointLight::GetLightRange( const PointLight& _light )
    {
        const float epsilon = 0.1f;	// Value at which we consider the light value null
        const float q = _light.mAttenuation[2];
        const float s = _light.mAttenuation[0];
        const float L = _light.mAttenuation[1];
        if ( q < epsilon )
        {	// first order linear differential equation
            return ( ( 1.f / epsilon ) - s ) / L;
        }
        else
        { // Second order linear differential equation
            float delta = L * L - 4 * q * ( s - 1 / epsilon );
            float sqrtDelta = std::sqrtf( delta );
            return ( -L + sqrtDelta ) / ( 2 * q );
        }
    }

	//========================================================================================================
	//========================================================================================================
	void PointLight::Save( const EcsComponent& _component, Json& _json )
	{
		const PointLight& pointLight = static_cast<const PointLight&>( _component );
		Serializable::SaveColor( _json, "ambiant", pointLight.mAmbiant );
		Serializable::SaveColor( _json, "diffuse", pointLight.mDiffuse );
		Serializable::SaveColor( _json, "specular", pointLight.mSpecular );
        Serializable::SaveVec3( _json,
                                "attenuation",
                                btVector3( pointLight.mAttenuation[0],
                                           pointLight.mAttenuation[1],
                                           pointLight.mAttenuation[2] ) );
	}

	//========================================================================================================
	//========================================================================================================
	void PointLight::Load( EcsComponent& _component, const Json& _json )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
		Serializable::LoadColor( _json, "ambiant", pointLight.mAmbiant );
		Serializable::LoadColor( _json, "diffuse", pointLight.mDiffuse );
		Serializable::LoadColor( _json, "specular", pointLight.mSpecular );

		btVector3 tmp;
		Serializable::LoadVec3( _json, "attenuation", tmp );
		pointLight.mAttenuation[0] = tmp[0];
		pointLight.mAttenuation[1] = tmp[1];
		pointLight.mAttenuation[2] = tmp[2];
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
            if( ImGui::Button( "##diffusecpl2" ) ) { pointLight.mDiffuse = Color::White; } ImGui::SameLine();
            ImGui::ColorEdit3( "diffuse##diffusepl2", pointLight.mDiffuse.Data(), ImGui::fanColorEditFlags );
            if( ImGui::Button( "##specularcpl2" ) ) { pointLight.mSpecular = Color::White ; } ImGui::SameLine();
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