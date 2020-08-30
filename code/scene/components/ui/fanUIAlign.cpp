#include "scene/components/ui/fanUIAlign.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIAlign::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Align16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIAlign::OnGui;
		_info.load        = &UIAlign::Load;
		_info.save        = &UIAlign::Save;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui align";
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UIAlign& align = static_cast<UIAlign&>( _component );
        align.mCorner = AlignCorner::TopLeft;
        align.mDirection = AlignDirection::Vertical;
        align.mUnitType = UnitType::Ratio;
        align.mOffset = glm::vec2( 0.f, 0.f );
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Save( const EcsComponent& _component, Json& _json )
	{
        const UIAlign& align = static_cast<const UIAlign&>( _component );
        Serializable::SaveInt( _json, "corner", align.mCorner );
        Serializable::SaveInt( _json, "direction", align.mDirection );
        Serializable::SaveInt( _json, "unitType", align.mUnitType );
        Serializable::SaveVec2( _json, "ratio", align.mOffset );
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Load( EcsComponent& _component, const Json& _json )
	{
        UIAlign& align = static_cast<UIAlign&>( _component );
        Serializable::LoadInt( _json, "corner", (int&)align.mCorner );
        Serializable::LoadInt( _json, "direction", (int&)align.mDirection );
        Serializable::LoadInt( _json, "unitType", (int&)align.mUnitType );
        Serializable::LoadVec2( _json, "ratio", align.mOffset );
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