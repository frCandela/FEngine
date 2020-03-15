#include "scene/ecs/components/fanTransformUI.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( UITransform2, "ui_transform" );

	//================================================================================================================================
	//================================================================================================================================
	void UITransform2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.onGui = &UITransform2::OnGui;
		_info.init = &UITransform2::Init;
		_info.load = &UITransform2::Load;
		_info.save = &UITransform2::Save;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform2::Init( ecComponent& _component )
	{
		UITransform2& transform = static_cast<UITransform2&>( _component );

		transform.scale = glm::vec2(1.f, 1.f);
		transform.position = glm::vec2( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform2::OnGui( ecComponent& _component )
	{
		UITransform2& transform = static_cast<UITransform2&>( _component );

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
	void UITransform2::Save( const ecComponent& _component, Json& _json )
	{
		const UITransform2& transform = static_cast<const UITransform2&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.position );
		Serializable::SaveVec2( _json, "scale", { transform.scale .x, transform.scale .y} );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform2::Load( ecComponent& _component, const Json& _json )
	{
		UITransform2& transform = static_cast<UITransform2&>( _component );

		btVector2 tmpPos;
		btVector2 tmpScale;
		Serializable::LoadVec2( _json, "position", tmpPos );
		Serializable::LoadVec2( _json, "scale", tmpScale );
		transform.position = { tmpPos[0], tmpPos[1] };
		transform.scale = { tmpScale[0], tmpScale[1] };
	}
}