#prama once

#include "scene/components/ui/fanUIText.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIText::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Text16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIText::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui text";
	}

    //========================================================================================================
	//========================================================================================================
	void UIText::OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
        UIText& text = static_cast<UIText&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
            if( ImGui::FanFontPtr("font", text.mFontPtr ) )
            {
                _world.AddTag<TagUIModified>( _entity );
            }

            if( ImGui::Button( "##clear_font_size" ) )
            {
                text.mSize = 18;
                _world.AddTag<TagUIModified>( _entity );
            }
            ImGui::SameLine();
            ImGui::DragInt( "size", &text.mSize, 1, 8, 300 );
            if( ImGui::IsItemDeactivatedAfterEdit() )
            {
                _world.AddTag<TagUIModified>( _entity );
            }

			// color
			if( ImGui::Button( "##clear_text" ) )
			{
                text.mText.clear();
                _world.AddTag<TagUIModified>( _entity );
			}
			ImGui::SameLine();
			static const size_t maxSize = 64;
			char mTextBuffer[maxSize];
			memcpy( mTextBuffer, text.mText.c_str(), std::min( maxSize, text.mText.size() + 1 ) );
			if( ImGui::InputText("text", mTextBuffer, 64 ) )
            {
                text.mText = mTextBuffer;
                _world.AddTag<TagUIModified>( _entity );
            }
		} ImGui::PopItemWidth();
	}
}