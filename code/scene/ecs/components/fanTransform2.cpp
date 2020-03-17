#include "scene/ecs/components/fanTransform2.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Transform2, "transform" );

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::TRANSFORM16;
		_info.onGui = &Transform2::OnGui;
		_info.init = &Transform2::Init;
		_info.save = &Transform2::Save;
		_info.load = &Transform2::Load;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::Init( ecComponent& _component )
	{
		Transform2& transform = static_cast<Transform2&>( _component );
		transform.transform.setIdentity();
		transform.scale = btVector3::One();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::OnGui( ecComponent& _transform )
	{
		Transform2& transform = static_cast<Transform2&>( _transform );
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
	void Transform2::Save( const ecComponent& _transform, Json& _json )
	{
		const Transform2& transform = static_cast<const Transform2&>( _transform );

		Serializable::SaveVec3( _json, "position", transform.GetPosition() );
		Serializable::SaveQuat( _json, "rotation", transform.GetRotationQuat() );
		Serializable::SaveVec3( _json, "scale", transform.scale );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::Load( ecComponent& _transform, const Json& _json )
 	{
		Transform2& transform = static_cast<Transform2&>( _transform );

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
	btVector3			Transform2::GetPosition() const		{ return transform.getOrigin(); }
	btVector3			Transform2::GetScale() const		{ return scale; }
	btQuaternion		Transform2::GetRotationQuat() const { return transform.getRotation(); }
	glm::mat4			Transform2::GetNormalMatrix() const	{ return glm::transpose( glm::inverse( GetModelMatrix() ) ); }
	btVector3			Transform2::Left() const			{ return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Left(); }
	btVector3			Transform2::Forward() const			{ return btTransform( GetRotationQuat() ) * btVector3::Forward(); }
	btVector3			Transform2::Up() const				{ return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Up(); }

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::LookAt( const btVector3& _target, const btVector3& _up )
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
	void Transform2::SetPosition( btVector3 _newPosition )
	{
		if( transform.getOrigin() != _newPosition )
		{
			transform.setOrigin( _newPosition );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::SetScale( btVector3 _newScale )
	{
		if( scale != _newScale )
		{
			scale = _newScale;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::SetRotationEuler( const btVector3 _rotation )
	{
		btQuaternion quat;
		quat.setEulerZYX( btRadians( _rotation.z() ), btRadians( _rotation.y() ), btRadians( _rotation.x() ) );
		SetRotationQuat( quat );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform2::GetRotationEuler() const
	{
		btVector3 euler;
		transform.getRotation().getEulerZYX( euler[2], euler[1], euler[0] );
		return btDegrees3( euler );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform2::SetRotationQuat( const btQuaternion _rotation )
	{
		if( transform.getRotation() != _rotation )
		{
			transform.setRotation( _rotation );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Transform2::GetModelMatrix() const
	{
		glm::vec3 position = ToGLM( transform.getOrigin() );
		glm::vec3 glmScale = ToGLM( scale );
		glm::quat rotation = ToGLM( transform.getRotation() );

		return glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), glmScale );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform2::TransformPoint( const btVector3 _point ) const
	{
		btVector3 transformedPoint = transform * ( scale * _point );
		return transformedPoint;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform2::InverseTransformPoint( const btVector3 _point ) const
	{
		const btVector3 invertScale( 1.f / scale[0], 1.f / scale[1], 1.f / scale[2] );
		btVector3 transformedPoint = invertScale * ( transform.inverse() * _point );
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform2::TransformDirection( const btVector3 _point ) const
	{
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = transform * _point;
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform2::InverseTransformDirection( const btVector3 _point ) const
	{
		const btVector3 invertScale( 1.f / scale[0], 1.f / scale[1], 1.f / scale[2] );
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = invertScale * ( transform.inverse() * _point );
		return transformedPoint;
	}
}