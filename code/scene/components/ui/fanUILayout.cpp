#include "scene/components/ui/fanUILayout.hpp"
#include "scene/components/ui/fanUITransform.hpp"

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
    ComponentPtr<UITransform>& UILayout::CreateItem( EcsWorld& _world, UILayout& _layout )
    {
        _layout.mTransforms.emplace_back();
        ComponentPtr<UITransform>& transformPtr = *_layout.mTransforms.rbegin();
        transformPtr.Init( _world );
        return transformPtr;
    }

	//========================================================================================================
	//========================================================================================================
	void UILayout::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        ComponentPtr<UITransform> transformPtr;
        transformPtr.Init( _world );
        layout.mTransforms.push_back( transformPtr );
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
        Serializable::SaveInt(_json, "count", (int)layout.mTransforms.size());
        for( int i = 0; i < (int)layout.mTransforms.size(); i++ )
        {
            Serializable::SaveComponentPtr( _json, std::to_string(i).c_str(), layout.mTransforms[i] );
        }
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Load( EcsComponent& _component, const Json& _json )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        fanAssert( ! layout.mTransforms.empty() );
        EcsWorld& world = *layout.mTransforms[0].world;
        layout.mTransforms.clear();

        int count = 0;
        Serializable::LoadInt(_json, "type", (int&)layout.mType );
        Serializable::LoadInt(_json, "gap", layout.mGap );
        Serializable::LoadInt(_json, "count", count );
        for( int i = 0; i < count; i++ )
        {
            ComponentPtr<UITransform>& transformPtr = UILayout::CreateItem( world, layout );
            Serializable::LoadComponentPtr( _json, std::to_string(i).c_str(), transformPtr );
        }
	}

    //========================================================================================================
    //========================================================================================================
    void UILayout::OnGui( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        UILayout& layout = static_cast<UILayout&>( _component );
        ImGui::Text("transforms: ");
        ImGui::Indent();
        ImGui::SameLine();
        if( ImGui::ButtonIcon(ImGui::IconType::PLUS8, {8,8} ) )
        {
            UILayout::CreateItem( _world, layout );
        }
        ImGui::SameLine();
        if(ImGui::ButtonIcon(ImGui::IconType::MINUS8, {8,8} ) && ! layout.mTransforms.empty() )
        {
            layout.mTransforms.pop_back();
        }
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
        for( int i = 0; i < layout.mTransforms.size(); i++ )
        {
            ImGui::PushID(i);
            ComponentPtr<UITransform> & transformPtr = layout.mTransforms[i];
            ImGui::FanComponent( "", transformPtr );
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::Unindent();
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
        ImGui::Combo("type", (int*)&layout.mType, "horizontal\0vertical\0\0");
        ImGui::DragInt( "gap", &layout.mGap, 1.f, -1, 100 );
        ImGui::PopItemWidth();
    }
}