#include "scene/components/ui/fanUILayout.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UILayout::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Layout16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UILayout::OnGui;
		_info.load        = &UILayout::Load;
		_info.save        = &UILayout::Save;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui layout";
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        layout.mType = Type::Horizontal;
        layout.mGap = -1;
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Save( const EcsComponent& _component, Json& _json )
	{
        const UILayout& layout = static_cast<const UILayout&>( _component );
        Serializable::SaveInt(_json, "type", (int)layout.mType );
        Serializable::SaveInt(_json, "gap", layout.mGap );
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Load( EcsComponent& _component, const Json& _json )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        Serializable::LoadInt(_json, "type", (int&)layout.mType );
        Serializable::LoadInt(_json, "gap", layout.mGap );
	}

    //========================================================================================================
    //========================================================================================================
    void UILayout::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        UILayout& layout = static_cast<UILayout&>( _component );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
        ImGui::Combo("type", (int*)&layout.mType, "horizontal\0vertical\0\0");
        ImGui::DragInt( "gap", &layout.mGap, 1.f, -1, 100 );
        ImGui::PopItemWidth();
    }
}