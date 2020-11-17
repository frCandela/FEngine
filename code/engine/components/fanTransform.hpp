#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btQuaternion.h"
#include "bullet/LinearMath/btTransform.h"

namespace fan
{
	//========================================================================================================
	// position, rotation and scale
	//========================================================================================================
	struct Transform : public EcsComponent
	{
		ECS_COMPONENT( Transform )
		static void SetInfo( EcsComponentInfo& _info );
		static void	Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		static void NetSave( const EcsComponent& _component, sf::Packet& _packet );
		static void NetLoad( EcsComponent& _component, sf::Packet& _packet );
		static void RollbackSave( const EcsComponent& _component, sf::Packet& _packet );
		static void RollbackLoad( EcsComponent& _component, sf::Packet& _packet );

		void SetPosition( btVector3 _newPosition );
		void SetScale( btVector3 _newScale );
		void SetRotationEuler( const btVector3 _rotation );
		void SetRotationQuat( const btQuaternion _rotation );
		void LookAt( const btVector3& _target, const btVector3& _up );

		const btVector3&	GetPosition() const;
		btVector3			GetScale() const;
		btQuaternion		GetRotationQuat() const;
		btVector3			GetRotationEuler() const;
		glm::mat4			GetModelMatrix() const;
		glm::mat4			GetNormalMatrix() const;

		btVector3 Left() const;
		btVector3 Forward() const;
		btVector3 Up() const;

		btVector3 TransformPoint( const btVector3 _point ) const;
		btVector3 TransformDirection( const btVector3 _point ) const;
		btVector3 InverseTransformPoint( const btVector3 _point ) const;
		btVector3 InverseTransformDirection( const btVector3 _point ) const;

		btTransform mTransform;
		btVector3   mScale;
	};
}