#include "scene/components/fanTransform.hpp"

#include "core/fanSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Transform, "transform" );

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::TRANSFORM16;
		_info.onGui = &Transform::OnGui;
		_info.init = &Transform::Init;
		_info.save = &Transform::Save;
		_info.load = &Transform::Load;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::Init( EcsWorld& _world, Component& _component )
	{
		Transform& transform = static_cast<Transform&>( _component );
		transform.transform.setIdentity();
		transform.scale = btVector3::One();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
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
			ImGui::DragFloat3( "scale", &transform.scale[0], 0.1f );

		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::Save( const Component& _component, Json& _json )
	{
		const Transform& transform = static_cast<const Transform&>( _component );

		Serializable::SaveVec3( _json, "position", transform.GetPosition() );
		Serializable::SaveQuat( _json, "rotation", transform.GetRotationQuat() );
		Serializable::SaveVec3( _json, "scale", transform.scale );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::Load( Component& _component, const Json& _json )
 	{
		Transform& transform = static_cast<Transform&>( _component );

 		btVector3 tmpVec;
 		btQuaternion tmpQuat;
 
 		Serializable::LoadVec3( _json, "position", tmpVec );
 		Serializable::LoadQuat( _json, "rotation", tmpQuat );
 		Serializable::LoadVec3( _json, "scale", transform.scale );
 
		transform.transform.setOrigin( tmpVec );
		transform.transform.setRotation( tmpQuat );
 	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3			Transform::GetPosition() const		{ return transform.getOrigin(); }
	btVector3			Transform::GetScale() const		{ return scale; }
	btQuaternion		Transform::GetRotationQuat() const { return transform.getRotation(); }
	glm::mat4			Transform::GetNormalMatrix() const	{ return glm::transpose( glm::inverse( GetModelMatrix() ) ); }
	btVector3			Transform::Left() const			{ return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Left(); }
	btVector3			Transform::Forward() const			{ return btTransform( GetRotationQuat() ) * btVector3::Forward(); }
	btVector3			Transform::Up() const				{ return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Up(); }

	//================================================================================================================================
	//================================================================================================================================
	void Transform::LookAt( const btVector3& _target, const btVector3& _up )
	{
		btVector3 forward = _target - transform.getOrigin();
		forward.normalize();
		btVector3 left = _up.cross( forward );
		left.normalize();

		transform.setBasis( btMatrix3x3( 
			left[0], _up[0], forward[0],
			left[1], _up[1], forward[1],
			left[2], _up[2], forward[2] ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetPosition( btVector3 _newPosition )
	{
		if( transform.getOrigin() != _newPosition )
		{
			transform.setOrigin( _newPosition );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetScale( btVector3 _newScale )
	{
		if( scale != _newScale )
		{
			scale = _newScale;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationEuler( const btVector3 _rotation )
	{
		btQuaternion quat;
		quat.setEulerZYX( btRadians( _rotation.z() ), btRadians( _rotation.y() ), btRadians( _rotation.x() ) );
		SetRotationQuat( quat );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::GetRotationEuler() const
	{
		btVector3 euler;
		transform.getRotation().getEulerZYX( euler[2], euler[1], euler[0] );
		return btDegrees3( euler );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationQuat( const btQuaternion _rotation )
	{
		if( transform.getRotation() != _rotation )
		{
			transform.setRotation( _rotation );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Transform::GetModelMatrix() const
	{
		glm::vec3 position = ToGLM( transform.getOrigin() );
		glm::vec3 glmScale = ToGLM( scale );
		glm::quat rotation = ToGLM( transform.getRotation() );

		return glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), glmScale );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::TransformPoint( const btVector3 _point ) const
	{
		btVector3 transformedPoint = transform * ( scale * _point );
		return transformedPoint;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::InverseTransformPoint( const btVector3 _point ) const
	{
		const btVector3 invertScale( 1.f / scale[0], 1.f / scale[1], 1.f / scale[2] );
		btVector3 transformedPoint = invertScale * ( transform.inverse() * _point );
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform::TransformDirection( const btVector3 _point ) const
	{
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = transform * _point;
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform::InverseTransformDirection( const btVector3 _point ) const
	{
		const btVector3 invertScale( 1.f / scale[0], 1.f / scale[1], 1.f / scale[2] );
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = invertScale * ( transform.inverse() * _point );
		return transformedPoint;
	}
}