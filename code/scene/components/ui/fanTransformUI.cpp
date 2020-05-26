#include "scene/components/ui/fanTransformUI.hpp"

#include "core/fanSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.onGui = &TransformUI::OnGui;
		_info.load = &TransformUI::Load;
		_info.save = &TransformUI::Save;
		_info.editorPath = "ui/";
		_info.name = "ui transform";
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

		transform.scale = glm::vec2(1.f, 1.f);
		transform.position = glm::vec2( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

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
	void TransformUI::Save( const EcsComponent& _component, Json& _json )
	{
		const TransformUI& transform = static_cast<const TransformUI&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.position );
		Serializable::SaveVec2( _json, "scale", transform.scale );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Load( EcsComponent& _component, const Json& _json )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );
		Serializable::LoadVec2( _json, "position", transform.position );
		Serializable::LoadVec2( _json, "scale", transform.scale );
	}
}