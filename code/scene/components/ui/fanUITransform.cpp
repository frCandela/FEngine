#include "scene/components/ui/fanUITransform.hpp"

#include "core/fanSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UITransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &UITransform::OnGui;
		_info.load = &UITransform::Load;
		_info.save = &UITransform::Save;
		_info.editorPath = "ui/";
		_info.name = "ui transform";
	}

	//========================================================================================================
	//========================================================================================================
	void UITransform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		UITransform& transform = static_cast<UITransform&>( _component );

		transform.mSize     = glm::ivec2( 100, 100);
		transform.mPosition = glm::ivec2( 0, 0 );
	}

	//========================================================================================================
	//========================================================================================================
	void UITransform::Save( const EcsComponent& _component, Json& _json )
	{
		const UITransform& transform = static_cast<const UITransform&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.mPosition );
		Serializable::SaveIVec2( _json, "size", transform.mSize );
	}

	//========================================================================================================
	//========================================================================================================
	void UITransform::Load( EcsComponent& _component, const Json& _json )
	{
		UITransform& transform = static_cast<UITransform&>( _component );
		Serializable::LoadIVec2( _json, "position", transform.mPosition );
		Serializable::LoadIVec2( _json,"size", transform.mSize );
	}

    //========================================================================================================
    //========================================================================================================
    void UITransform::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UITransform& transform = static_cast<UITransform&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            if( ImGui::Button( "##TransUIPos" ) ){	transform.mPosition = glm::vec2( 0, 0 ); }
            ImGui::SameLine();
            ImGui::DragInt2( "position", &transform.mPosition.x, 1, 0 );

            if( ImGui::Button( "##resetUIsize" ) ) { transform.mSize = glm::ivec2( 100, 100 ); }
            ImGui::SameLine();
            ImGui::DragInt2( "size##sizeUI", &transform.mSize.x, 0.1f );
        }
        ImGui::PopItemWidth();
    }
}