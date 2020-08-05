#include "scene/components/ui/fanTransformUI.hpp"

#include "core/fanSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &TransformUI::OnGui;
		_info.load = &TransformUI::Load;
		_info.save = &TransformUI::Save;
		_info.editorPath = "ui/";
		_info.name = "ui transform";
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

		transform.mSize     = glm::ivec2( 100, 100);
		transform.mPosition = glm::ivec2( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );

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

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Save( const EcsComponent& _component, Json& _json )
	{
		const TransformUI& transform = static_cast<const TransformUI&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.mPosition );
		Serializable::SaveIVec2( _json, "size", transform.mSize );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TransformUI::Load( EcsComponent& _component, const Json& _json )
	{
		TransformUI& transform = static_cast<TransformUI&>( _component );
		Serializable::LoadIVec2( _json, "position", transform.mPosition );
		Serializable::LoadIVec2( _json,"size", transform.mSize );
	}
}