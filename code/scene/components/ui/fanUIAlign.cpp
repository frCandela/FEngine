#include "scene/components/ui/fanUIAlign.hpp"
#include "scene/components/ui/fanUITransform.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIAlign::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NONE;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &UIAlign::OnGui;
		_info.load = &UIAlign::Load;
		_info.save = &UIAlign::Save;
		_info.editorPath = "ui/";
		_info.name = "ui align";
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UIAlign& align = static_cast<UIAlign&>( _component );
        align.mCorner = AlignCorner::TopLeft;
        align.mDirection = AlignDirection::Vertical;
        align.mRatio = 0.f; // [0.f, 1.f]
        align.mParent.Init( _world );
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Save( const EcsComponent& _component, Json& _json )
	{
        const UIAlign& align = static_cast<const UIAlign&>( _component );
        Serializable::SaveComponentPtr( _json, "parent", align.mParent );
        Serializable::SaveInt( _json, "corner", align.mCorner );
        Serializable::SaveInt( _json, "direction", align.mDirection );
        Serializable::SaveFloat( _json, "ratio", align.mRatio );
	}

	//========================================================================================================
	//========================================================================================================
	void UIAlign::Load( EcsComponent& _component, const Json& _json )
	{
        UIAlign& align = static_cast<UIAlign&>( _component );
        Serializable::LoadComponentPtr( _json, "parent", align.mParent );
        Serializable::LoadInt( _json, "corner", (int&)align.mCorner );
        Serializable::LoadInt( _json, "direction", (int&)align.mDirection );
        Serializable::LoadFloat( _json, "ratio", align.mRatio );
    }

    //========================================================================================================
    //========================================================================================================
    void UIAlign::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UIAlign& align = static_cast<UIAlign&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );

        ImGui::FanComponent("parent", align.mParent );
        ImGui::Combo("corner", (int*)&align.mCorner, "TopLeft\0TopRight\0BottomLeft\0BottomRight\0\0" );
        ImGui::Combo("direction", (int*)&align.mDirection, "Horizontal\0Vertical\0\0" );
        ImGui::DragFloat("ratio", &align.mRatio, 0.01f, 0.f, 1.f );

        ImGui::PopItemWidth();

    }
}