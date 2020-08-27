#include "scene/components/fanMaterial.hpp"
#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Material::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Material16;
		_info.mGroup      = EngineGroups::SceneRender;
		_info.onGui       = &Material::OnGui;
		_info.load        = &Material::Load;
		_info.save        = &Material::Save;
		_info.mEditorPath = "/";
		_info.mName       = "material";
	}

	//========================================================================================================
	//========================================================================================================
	void Material::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Material& material = static_cast<Material&>( _component );

		material.mTexture   = nullptr;
		material.mShininess = 1;
		material.mColor     = Color::sWhite;
	}

	//========================================================================================================
	//========================================================================================================
	void Material::Save( const EcsComponent& _component, Json& _json )
	{
		const Material& material = static_cast<const Material&>( _component );

		Serializable::SaveUInt( _json, "shininess", material.mShininess );
		Serializable::SaveColor( _json, "color", material.mColor );
		Serializable::SaveTexturePtr( _json, "material", material.mTexture );
	}

	//========================================================================================================
	//========================================================================================================
	void Material::Load( EcsComponent& _component, const Json& _json )
	{
		Material& material = static_cast<Material&>( _component );

		Serializable::LoadUInt( _json, "shininess", material.mShininess );
		Serializable::LoadColor( _json, "color", material.mColor );
		Serializable::LoadTexturePtr( _json, "material", material.mTexture );
	}

    //========================================================================================================
    //========================================================================================================
    void Material::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Material& material = static_cast<Material&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
        {
            // Filter color
            if( ImGui::Button( "##color" ) ) { material.mColor = Color::sWhite; } ImGui::SameLine();
            ImGui::ColorEdit4( "color", material.mColor.Data(), ImGui::fanColorEditFlags );

            if( ImGui::Button( "##shininess" ) ) { material.mShininess = 1; } ImGui::SameLine();
            ImGui::DragInt( "shininess", (int*)&material.mShininess, 1, 1, 256 );
            ImGui::SameLine(); ImGui::FanShowHelpMarker( "sharpness of the specular reflection" );
        }
        ImGui::PopItemWidth();

        ImGui::FanTexturePtr( "mat texture", material.mTexture );
    }
}