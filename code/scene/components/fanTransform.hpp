#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//================================================================================================================================
	//================================================================================================================================
	struct Transform : public Component
	{
		DECLARE_COMPONENT( Transform )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void	Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		void SetPosition( btVector3 _newPosition );
		void SetScale( btVector3 _newScale );
		void SetRotationEuler( const btVector3 _rotation );
		void SetRotationQuat( const btQuaternion _rotation );
		void LookAt( const btVector3& _target, const btVector3& _up );

		btVector3			GetPosition() const;
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

		btTransform transform;
		btVector3   scale;
	};
	static constexpr size_t sizeof_transform2 = sizeof( Transform );
}