#include "scene/components/fanFollowTransform.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "editor/fanModals.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::TRANSFORM16;
		_info.onGui = &FollowTransform::OnGui;
		_info.save = &FollowTransform::Save;
		_info.load = &FollowTransform::Load;
		_info.editorPath = "/";
		_info.name = "follow transform";
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		// clear
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		followTransform.targetTransform.Init( _world );
		followTransform.localTransform = btTransform::getIdentity();
		followTransform.locked = true;
	}

	//================================================================================================================================
	// calculates the new local transform from the follower & target transforms
	//================================================================================================================================
	void FollowTransform::UpdateLocalTransform( EcsWorld& _world, EcsEntity _entityID )
	{
		assert( _world.HasComponent<FollowTransform>( _entityID ) );

		FollowTransform& follower = _world.GetComponent<FollowTransform>( _entityID );

		if( follower.targetTransform != nullptr && _world.HasComponent<Transform>( _entityID ) )
		{
			Transform& target = *follower.targetTransform;
			Transform& follow = _world.GetComponent<Transform>( _entityID );
			follower.localTransform = S_InitFollowTransforms::GetLocalTransform( target.transform, follow.transform );
		}
		else
		{
			follower.localTransform = btTransform::getIdentity();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		if( ImGui::FanComponent<Transform>( "target transform", followTransform.targetTransform ) )
		{
			UpdateLocalTransform( _world, _entityID );
		}

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

		enum ImGui::IconType icon = followTransform.locked ? ImGui::LOCK_CLOSED16 : ImGui::LOCK_OPEN16;
		if( ImGui::ButtonIcon( icon, { 16,16 } ) )
		{
			followTransform.locked = !followTransform.locked;
			UpdateLocalTransform( _world, _entityID );
		}
		ImGui::SameLine();
		ImGui::Text( "lock transform " );
		ImGui::SameLine();
		ImGui::FanShowHelpMarker( "press L to toggle" );

		// draw the local transform offset
		if( followTransform.targetTransform != nullptr && _world.HasComponent<Transform>( _entityID ) )
		{
			btTransform& target = followTransform.targetTransform->transform;
			btTransform& local = followTransform.localTransform;
			btTransform t = target * local;
			RendererDebug::Get().DebugLine( target.getOrigin(), t.getOrigin(), Color::Red );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Save( const EcsComponent& _component, Json& _json )
	{
		const FollowTransform& followTransform = static_cast<const FollowTransform&>( _component );
		Serializable::SaveComponentPtr( _json, "target_transform", followTransform.targetTransform );
		Serializable::SaveVec3( _json, "local_position", followTransform.localTransform.getOrigin() );
		Serializable::SaveQuat( _json, "local_rotation", followTransform.localTransform.getRotation() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Load( EcsComponent& _component, const Json& _json )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		Serializable::LoadComponentPtr( _json, "target_transform", followTransform.targetTransform );
		btVector3 tmpVec;
		btQuaternion tmpQuat;
		Serializable::LoadVec3( _json, "local_position", tmpVec );
		Serializable::LoadQuat( _json, "local_rotation", tmpQuat );
		followTransform.localTransform.setOrigin( tmpVec );
		followTransform.localTransform.setRotation( tmpQuat );
	}
}