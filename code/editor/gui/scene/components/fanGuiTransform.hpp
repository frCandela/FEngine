#pragma once

#include "scene/components/fanTransform.hpp"

namespace fan
{	
	//========================================================================================================
	//========================================================================================================
	void Transform::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon        = ImGui::IconType::Transform16;
		_info.mGroup       = EngineGroups::Scene;
		_info.onGui        = &Transform::OnGui;
		_info.mEditorPath  = "/";
		_info.mName        = "transform";
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		Transform& transform = static_cast<Transform&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			// Position
			if( ImGui::Button( "##TransPos" ) )
			{
				transform.SetPosition( btVector3( 0, 0, 0 ) );
			} ImGui::SameLine();
			btVector3 position = transform.GetPosition();
			if( ImGui::DragFloat3( "position", &position[0], 0.1f ) )
			{
				transform.SetPosition( position );
			}

			// rotation
			if( ImGui::Button( "##TransRot" ) )
			{
				transform.SetRotationQuat( btQuaternion::getIdentity() );
			} ImGui::SameLine();
			btVector3 rotation = transform.GetRotationEuler();
			if( ImGui::DragFloat3( "rotation", &rotation[0], 0.1f ) )
			{
				transform.SetRotationEuler( rotation );
			}

			// Scale
			if( ImGui::Button( "##TransScale" ) )
			{
				transform.SetScale( btVector3( 1, 1, 1 ) );
			} ImGui::SameLine();
			ImGui::DragFloat3( "scale", &transform.mScale[0], 0.1f );
		}
		ImGui::PopItemWidth();
	}

}