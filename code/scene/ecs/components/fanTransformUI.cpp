#include "scene/ecs/components/fanTransformUI.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( UITransform, "ui_transform" );

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.onGui = &UITransform::OnGui;
		_info.init = &UITransform::Init;
		_info.load = &UITransform::Load;
		_info.save = &UITransform::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::Init( Component& _component )
	{
		UITransform& transform = static_cast<UITransform&>( _component );

		transform.scale = glm::vec2(1.f, 1.f);
		transform.position = glm::vec2( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::OnGui( Component& _component )
	{
		UITransform& transform = static_cast<UITransform&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			if( ImGui::Button( "##TransUIPos" ) ){	transform.position = glm::vec2( 0, 0 ); } 
			ImGui::SameLine();
			ImGui::DragFloat2( "position", &transform.position.x, 1, 0 );

			if( ImGui::Button( "##resetUIscale" ) ) { transform.scale = glm::vec2( 1.f, 1.f ); }
			ImGui::SameLine();
			ImGui::DragFloat2( "scale##scaleUI", &transform.scale.x, 0.1f );
		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::Save( const Component& _component, Json& _json )
	{
		const UITransform& transform = static_cast<const UITransform&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.position );
		Serializable::SaveVec2( _json, "scale", { transform.scale .x, transform.scale .y} );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::Load( Component& _component, const Json& _json )
	{
		UITransform& transform = static_cast<UITransform&>( _component );

		btVector2 tmpPos;
		btVector2 tmpScale;
		Serializable::LoadVec2( _json, "position", tmpPos );
		Serializable::LoadVec2( _json, "scale", tmpScale );
		transform.position = { tmpPos[0], tmpPos[1] };
		transform.scale = { tmpScale[0], tmpScale[1] };
	}
}