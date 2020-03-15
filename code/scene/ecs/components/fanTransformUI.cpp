#include "scene/ecs/components/fanTransformUI.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( TransformUI, "ui_transform" );

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.onGui = &TransformUI::OnGui;
		_info.init = &TransformUI::Init;
		_info.load = &TransformUI::Load;
		_info.save = &TransformUI::Save;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Init( ecComponent& _component )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

		transform.size = glm::vec2(256, 256);
		transform.position = glm::vec2( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::OnGui( ecComponent& _component )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			if( ImGui::Button( "##TransUIPos" ) ){	transform.position = glm::ivec2( 0, 0 ); } 
			ImGui::SameLine();
			ImGui::DragInt2( "position", &transform.position.x, 1, 0 );

			if( ImGui::Button( "##TransUIsize" ) ) { transform.size = glm::ivec2( 256, 256 ); }
			ImGui::SameLine();
			ImGui::DragInt2( "size##sizeUI", &transform.size.x,1, 0, 10000000 );
		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Save( const ecComponent& _component, Json& _json )
	{
		const TransformUI& transform = static_cast<const TransformUI&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.position );
		Serializable::SaveIVec2( _json, "size", transform.size );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Load( ecComponent& _component, const Json& _json )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

		Serializable::LoadIVec2( _json, "position", transform.position );
		Serializable::LoadIVec2( _json, "size", transform.size );
	}
}