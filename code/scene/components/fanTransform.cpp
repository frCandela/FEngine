#include "scene/components/fanTransform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/memory/fanSerializable.hpp"
#include "core/math/fanMathUtils.hpp"
#include "network/fanPacket.hpp"

namespace fan
{	
	//========================================================================================================
	//========================================================================================================
	void Transform::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon        = ImGui::IconType::Transform16;
		_info.mGroup       = EngineGroups::Scene;
		_info.onGui        = &Transform::OnGui;
		_info.save         = &Transform::Save;
		_info.load         = &Transform::Load;
		_info.netSave      = &Transform::NetSave;
		_info.netLoad      = &Transform::NetLoad;
		_info.rollbackLoad = &Transform::RollbackLoad;
		_info.rollbackSave = &Transform::RollbackSave;
		_info.mEditorPath  = "/";
		_info.mName        = "transform";
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Transform& transform = static_cast<Transform&>( _component );
		transform.mTransform.setIdentity();
		transform.mScale = btVector3::One();
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::Save( const EcsComponent& _component, Json& _json )
	{
		const Transform& transform = static_cast<const Transform&>( _component );

		Serializable::SaveVec3( _json, "position", transform.GetPosition() );
		Serializable::SaveQuat( _json, "rotation", transform.GetRotationQuat() );
		Serializable::SaveVec3( _json, "scale", transform.mScale );
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::Load( EcsComponent& _component, const Json& _json )
 	{
		Transform& transform = static_cast<Transform&>( _component );

 		btVector3 tmpVec;
 		btQuaternion tmpQuat;
 
 		Serializable::LoadVec3( _json, "position", tmpVec );
 		Serializable::LoadQuat( _json, "rotation", tmpQuat );
 		Serializable::LoadVec3( _json, "scale", transform.mScale );
 
		transform.mTransform.setOrigin( tmpVec );
		transform.mTransform.setRotation( tmpQuat );
 	}

	//========================================================================================================
	//========================================================================================================
	void Transform::RollbackSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const Transform& transform = static_cast<const Transform&>( _component );
		const btVector3& position = transform.GetPosition();
		const btVector3  rotation = transform.GetRotationEuler();

		_packet << position[0] << position[2];
		_packet << rotation[0] << rotation[1] << rotation[2];
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::RollbackLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		Transform& transform = static_cast<Transform&>( _component );
		btVector3 position( 0, 0, 0 );
		btVector3 rotation( 0, 0, 0 );
		_packet >> position[0] >> position[2];
		_packet >> rotation[0] >> rotation[1] >> rotation[2];
		transform.SetPosition( position );
		transform.SetRotationEuler( rotation );
	}

	//========================================================================================================
	//========================================================================================================
	const btVector3&	Transform::GetPosition() const	{ return mTransform.getOrigin(); }
	btVector3			Transform::GetScale() const		{ return mScale; }
	btQuaternion		Transform::GetRotationQuat() const { return mTransform.getRotation(); }
    glm::mat4 Transform::GetNormalMatrix() const
    {
	    return glm::transpose( glm::inverse( GetModelMatrix() ) );
    }
    btVector3 Transform::Left() const
    {
        return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Left();
    }
    btVector3 Transform::Forward() const { return btTransform( GetRotationQuat() ) * btVector3::Forward(); }
    btVector3 Transform::Up() const
    {
        return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) *
               btVector3::Up();
    }

	//========================================================================================================
	//========================================================================================================
	void Transform::LookAt( const btVector3& _target, const btVector3& _up )
	{
		btVector3 forward = _target - mTransform.getOrigin();
		forward.normalize();
		btVector3 left = _up.cross( forward );
		left.normalize();

		mTransform.setBasis( btMatrix3x3(
			left[0], _up[0], forward[0],
			left[1], _up[1], forward[1],
			left[2], _up[2], forward[2] ) );
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::SetPosition( btVector3 _newPosition )
	{
		if( mTransform.getOrigin() != _newPosition )
		{
			mTransform.setOrigin( _newPosition );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::SetScale( btVector3 _newScale )
	{
		if( mScale != _newScale )
		{
            mScale = _newScale;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::SetRotationEuler( const btVector3 _rotation )
	{
		btQuaternion quat;
        quat.setEulerZYX( btRadians( _rotation.z() ),
                          btRadians( _rotation.y() ),
                          btRadians( _rotation.x() ) );
		SetRotationQuat( quat );
	}

	//========================================================================================================
	//========================================================================================================
	btVector3 Transform::GetRotationEuler() const
	{
		btVector3 euler;
		mTransform.getRotation().getEulerZYX( euler[2], euler[1], euler[0] );
		return btDegrees3( euler );
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::SetRotationQuat( const btQuaternion _rotation )
	{
		if( mTransform.getRotation() != _rotation )
		{
			mTransform.setRotation( _rotation );
		}
	}

	//========================================================================================================
	//========================================================================================================
	glm::mat4 Transform::GetModelMatrix() const
	{
		glm::vec3 position = ToGLM( mTransform.getOrigin() );
		glm::vec3 glmScale = ToGLM( mScale );
		glm::quat rotation = ToGLM( mTransform.getRotation() );

        return glm::translate( glm::mat4( 1.f ), position ) *
               glm::mat4_cast( rotation ) *
               glm::scale( glm::mat4( 1.f ), glmScale );
	}

	//========================================================================================================
	//========================================================================================================
	btVector3 Transform::TransformPoint( const btVector3 _point ) const
	{
		btVector3 transformedPoint = mTransform * ( mScale * _point );
		return transformedPoint;
	}

	//========================================================================================================
	//========================================================================================================
	btVector3 Transform::InverseTransformPoint( const btVector3 _point ) const
	{
		const btVector3 invertScale( 1.f / mScale[0], 1.f / mScale[1], 1.f / mScale[2] );
		btVector3 transformedPoint = invertScale * ( mTransform.inverse() * _point );
		return transformedPoint;
	}

	//========================================================================================================
	// No translation applied
	//========================================================================================================
	btVector3 Transform::TransformDirection( const btVector3 _point ) const
	{
		const btTransform rotationTransform( GetRotationQuat() );
		btVector3 transformedPoint = rotationTransform * _point;
		return transformedPoint;
	}

	//========================================================================================================
	// No translation applied
	//========================================================================================================
	btVector3 Transform::InverseTransformDirection( const btVector3 _point ) const
	{
		const btVector3 invertScale( 1.f / mScale[0], 1.f / mScale[1], 1.f / mScale[2] );
		const btTransform rotationTransform( GetRotationQuat() );
		btVector3 transformedPoint = invertScale * ( rotationTransform.inverse() * _point );
		return transformedPoint;
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::NetSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const Transform& transform = static_cast<const Transform&>( _component );
		const btVector3 position = transform.GetPosition();
		const float yAxis = transform.GetRotationQuat().getAxis().y();
		float rotation = btDegrees( transform.GetRotationQuat().getAngle() );
		if( yAxis < 0.f )
		{
			rotation = -rotation;
		}

		_packet << position[0] << position[2];
		_packet << rotation;
	}

	//========================================================================================================
	//========================================================================================================
	void Transform::NetLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		Transform& transform = static_cast<Transform&>( _component );
		btVector3 position( 0.f, 0.f, 0.f );
		float rotation;
		_packet >> position[0] >> position[2];
		_packet >> rotation;

		transform.SetRotationEuler( btVector3( 0.f, rotation, 0.f ) );
		transform.SetPosition( position );
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