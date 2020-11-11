#prama once

#include "scene/components/ui/fanUIAlign.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIAlign::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Align16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIAlign::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui align";
	}

    //========================================================================================================
    //========================================================================================================
    void UIAlign::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UIAlign& align = static_cast<UIAlign&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );

        ImGui::Combo("corner", (int*)&align.mCorner, "TopLeft\0TopRight\0BottomLeft\0BottomRight\0\0" );
        ImGui::Combo("direction", (int*)&align.mDirection, "Horizontal\0Vertical\0HorizontalVertical\0\0" );
        ImGui::Combo("unit type", (int*)&align.mUnitType, "Ratio\0Pixels\0\0" );
        ImGui::PopItemWidth();

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        if( ImGui::Button( "##resetRatio" ) ){	align.mOffset = glm::vec2( 0, 0 ); }
        ImGui::SameLine();

        if( align.mUnitType == UIAlign::Ratio )
        {
            switch( align.mDirection )
            {
                case Horizontal:
                    ImGui::DragFloat( "offset ratio", &align.mOffset.x, 0.01f, 0.f, 1.f );
                    break;
                case Vertical:
                    ImGui::DragFloat( "offset ratio", &align.mOffset.y, 0.01f, 0.f, 1.f );
                    break;
                case HorizontalVertical:
                    ImGui::DragFloat2( "offset ratio", &align.mOffset.x, 0.01f, 0.f, 1.f );
                    break;
                default:
                    fanAssert( false );
                    break;
            }
        }
        else
        {
            fanAssert( align.mUnitType == UIAlign::Pixels );
            switch( align.mDirection )
            {
                case Horizontal:         ImGui::DragFloat( "offset pixels", &align.mOffset.x, 1.f ); break;
                case Vertical:           ImGui::DragFloat( "offset pixels", &align.mOffset.y, 1.f);  break;
                case HorizontalVertical: ImGui::DragFloat2( "offset pixels", &align.mOffset.x, 1.f); break;
                default: fanAssert( false ); break;
            }
        }
        ImGui::PopItemWidth();
    }
}