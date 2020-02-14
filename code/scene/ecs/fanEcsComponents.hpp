#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/meta/fanTypeList.hpp"
#include "core/math/shapes/fanAABB.hpp"
#include "core/math/shapes/fanConvexHull.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{

	class Mesh;

	using ecsEntity = uint32_t;
	const ecsEntity ecsNullEntity = std::numeric_limits< ecsEntity >::max();

	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct ecsIComponent {
	};

	//================================
	struct ecsTranform : ecsIComponent
	{
		static const char* s_name;
		void Init() { transform.setIdentity(); }
		void Clear(){}

		btTransform transform;
	};

	//================================
	struct ecsPosition : ecsIComponent
	{
		static const char* s_name;
		void Init() { position = btVector3::Zero(); }
		void Clear(){}

		btVector3	position = btVector3::Zero();
	};

	//================================
	struct ecsRotation : ecsIComponent
	{
		static const char* s_name;
		void Init() { rotation = btQuaternion::getIdentity(); }
		void Clear(){}

		btQuaternion	rotation = btQuaternion::getIdentity();
	};

	//================================
	struct ecsScaling : ecsIComponent
	{
		static const char* s_name;
		void Init() { scale = btVector3::One(); }
		void Clear(){}

		btVector3		scale = btVector3::One();
	};

	//================================
	struct ecsMovement : ecsIComponent
	{
		static const char* s_name;
		void Init() { speed = btVector3::Zero(); }
		void Clear(){}

		btVector3		speed = btVector3::Zero();
	};

	//================================
	struct ecsParticle : ecsIComponent
	{
		static const char* s_name;
		void Init()
		{
			color = Color::Red;
			durationLeft = 1.f;
		}
		void Clear(){}

		fan::Color	color = Color::Red;
		float		durationLeft = 1.f;
	};

	//================================ @todo make this a tag ?
	struct ecsSunlightParticleOcclusion : ecsIComponent
	{
		static const char* s_name;
		void Init()	{}
		void Clear(){}
	};

	//================================
	struct ecsAABB : ecsIComponent
	{
		static const char* s_name;
		void Init() { aabb.Clear(); }
		void Clear(){}

		AABB aabb;
	};

	//================================
	struct ecsMesh : ecsIComponent
	{
		static const char* s_name;
		void Init()
		{
			static_cast< ResourcePtr<Mesh> * >( &mesh )->SetNull();
			renderID = -1;
		}
		void Clear() { mesh.SetNull(); }

		MeshPtr mesh;
		int renderID = -1;
	};

	//================================
	struct ecsDirLight : ecsIComponent
	{
		static const char* s_name;
		void Init()
		{
			ambiant = Color::Black;
			diffuse = Color::White;
			specular = Color::White;
		}
		void Clear() {}

		Color ambiant = Color::Black;
		Color diffuse = Color::White;
		Color specular = Color::White;
	};

	//================================
	struct ecsPointLight : ecsIComponent
	{
		static const char* s_name;
		void Init()
		{
			ambiant = Color::White;
			diffuse = Color::White;
			specular = Color::White;
			attenuation[ 0 ] = 0.f; attenuation[ 1 ] = 0.f; attenuation[ 2 ] = 0.1f;
		}
		void Clear(){}

		Color ambiant = Color::White;
		Color diffuse = Color::White;
		Color specular = Color::White;
		float attenuation[ 3 ] = { 0.f,0.f,0.1f };
	};

	//================================
	struct ecsMaterial : ecsIComponent
	{
		static const char* s_name;
		void Init()
		{
			static_cast< ResourcePtr< Texture > * >( &texture )->SetNull();
			shininess = 1;
			color = Color::White;
		}
		void Clear() { texture.SetNull();  }

		TexturePtr 	texture;
		uint32_t	shininess = 1;
		Color		color = Color::White;
	};

	//================================
	struct ecsPlanet : ecsIComponent
	{
		static const char* s_name;
		void Init()
		{
			time = 0.f;
			speed = 1.f;
			radius = 1.f;
			phase = 0.f;
		}
		void Clear(){}

		float time = 0.f;
		float speed = 1.f;
		float radius = 1.f;
		float phase = 0.f;
	};

	//================================
	struct ecsRigidbody : ecsIComponent
	{
		static const char* s_name;
		char bufferRigidbody[ sizeof( btRigidBody ) ]; // dummy btRigidBody memory to bypass btRigidBody constructor

		btRigidBody* Init( const btRigidBody::btRigidBodyConstructionInfo& constructionInfo )
		{
			return new( bufferRigidbody ) btRigidBody( constructionInfo );
		}
		void Clear(){}

		inline btRigidBody& Get() { return *reinterpret_cast< btRigidBody* >( bufferRigidbody ); }

	}; static_assert( sizeof( ecsRigidbody ) == sizeof( btRigidBody ) );

	//================================
	struct ecsMotionState : ecsIComponent
	{
		static const char* s_name;
		char bufferMotionState[ sizeof( btDefaultMotionState ) ]; // dummy btMotionState memory to bypass btDefaultMotionState constructor

		btDefaultMotionState* Init( const btTransform& _startTrans = btTransform::getIdentity(),
									const btTransform& _centerOfMassOffset = btTransform::getIdentity() )
		{
			return new( bufferMotionState ) btDefaultMotionState( _startTrans, _centerOfMassOffset );
		}
		void Clear(){}

		inline btDefaultMotionState& Get() { return *reinterpret_cast< btDefaultMotionState* >( bufferMotionState ); }

	}; static_assert( sizeof( ecsMotionState ) == sizeof( btDefaultMotionState ) );

	//================================
	struct ecsSphereShape : ecsIComponent
	{
		static const char* s_name;
		char bufferSphereShape[ sizeof( btSphereShape ) ]; // dummy btSphereShape memory to bypass btDefaultMotionState constructor

		btSphereShape* Init( const float _radius = 1.f )
		{
			return new( bufferSphereShape ) btSphereShape( _radius );
		}
		void Clear(){}

		inline btSphereShape& Get() { return *reinterpret_cast< btSphereShape* >( bufferSphereShape ); }

	}; static_assert( sizeof( ecsSphereShape ) == sizeof( btSphereShape ) );

	//================================
	struct ecsBoxShape : ecsIComponent
	{
		static const char* s_name;
		char bufferBoxShape[ sizeof( btBoxShape ) ]; // dummy btBoxShape memory to bypass btDefaultMotionState constructor

		btBoxShape* Init( const btVector3 _boxHalfExtents =  btVector3( 0.5f, 0.5f, 0.5f ) )
		{
			return new( bufferBoxShape ) 	btBoxShape( _boxHalfExtents );
		}
		void Clear(){}

		inline 	btBoxShape& Get() { return *reinterpret_cast< btBoxShape* >( bufferBoxShape ); }

	}; static_assert( sizeof( ecsBoxShape ) == sizeof( btBoxShape ) );

	//================================
	struct ecsFlags : ecsIComponent
	{
		static const char* s_name;
		void Init() { flags = 0; }
		void Clear(){}

		uint32_t flags = 0;

		enum Flag
		{
			NONE = 1 << 0, NO_DELETE = 1 << 1, OUTDATED_TRANSFORM = 1 << 3,
			OUTDATED_MATERIAL = 1 << 4, OUTDATED_LIGHT = 1 << 5, OUTDATED_AABB = 1 << 6,
			NO_AABB_UPDATE = 1 << 7
		};
	};

	//================================
	struct ecsEditorFlags : ecsIComponent
	{
		static const char* s_name;
		void Init() { flags = 0; }
		void Clear(){}

		uint32_t flags = 0;

		enum Flag
		{
			NONE = 1 << 0, NO_DELETE = 1 << 1, NOT_SAVED = 1 << 2, NOT_UI_INSTANCIABLE = 1 << 2, ALWAYS_PLAY_ACTORS = 1 << 3
		};
	};

	//================================
	struct ecsBullet : ecsIComponent
	{
		static const char* s_name;
		void Init( const float _durationLeft, const float _damage )
		{
			durationLeft = _durationLeft;
			damage = _damage;
		}
		void Clear(){}

		float durationLeft = 1.f;
		float damage = 5.f;
	};

	//================================
	class Gameobject;
	struct ecsGameobject : ecsIComponent
	{
		static const char* s_name;
		void Init( Gameobject* _gameobject )
		{
			gameobject = _gameobject;
		}
		void Clear(){}

		Gameobject* gameobject = nullptr;
	};

	//================================
	//================================
	using ecsComponents = meta::TypeList <
		ecsTranform
		, ecsPosition
		, ecsRotation
		, ecsMovement
		, ecsParticle
		, ecsSunlightParticleOcclusion
		, ecsScaling
		, ecsAABB
		, ecsMesh
		, ecsPointLight
		, ecsDirLight
		, ecsMaterial
		, ecsPlanet
		, ecsRigidbody
		, ecsMotionState
		, ecsSphereShape
		, ecsBoxShape
		, ecsFlags
		, ecsEditorFlags
		, ecsBullet
		, ecsGameobject
	> ;

	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< ecsIComponent, _type >::value; };
}