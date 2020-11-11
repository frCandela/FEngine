#prama once

#include "scene/components/ui/fanUIRenderer.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::UiMeshRenderer16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIRenderer::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui renderer";
	}

    //========================================================================================================
	//========================================================================================================
	void UIRenderer::OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		UIRenderer& ui = static_cast<UIRenderer&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// color
			if( ImGui::Button( "##TransPos" ) )
			{
				ui.mColor = Color::sWhite;
			} ImGui::SameLine();
			ImGui::ColorEdit4( "color", ui.mColor.Data(), ImGui::fanColorEditFlags );

			// texture
			ImGui::FanTexturePtr( "ui texture", ui.mTexture );

			// enabled
            bool isEnabled = _world.HasTag<TagUIVisible>( _entity );
            if( ImGui::Checkbox("visible", &isEnabled ) )
            {
                if( isEnabled ) { _world.AddTag<TagUIVisible>(_entity);     }
                else            { _world.RemoveTag<TagUIVisible>(_entity);  }
            }

            if( ImGui::Button( "##depth ui" ) ){	ui.mDepth = 0 ; }
            ImGui::SameLine();
			ImGui::DragInt("depth", &ui.mDepth, 1, 0, 1024 );

		} ImGui::PopItemWidth();
	}
}