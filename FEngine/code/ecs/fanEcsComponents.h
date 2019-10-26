#pragma once

#include "core/meta/fanTypeList.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/shapes/fanConvexHull.h"

namespace fan {

	class Mesh;
	class Texture;

	using ecsEntity = uint32_t;
	const ecsEntity ecsNullEntity = std::numeric_limits< ecsEntity >::max();

	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct ecsIComponent {};

	//================================
	struct ecsTranform : ecsIComponent {
		static const char *  s_name;
		void Init() { transform.setIdentity(); }

		btTransform transform;
	};

	//================================
	struct ecsPosition : ecsIComponent {
		static const char *  s_name;
		void Init() { position = btVector3::Zero(); }

		btVector3	position = btVector3::Zero();
	};

	//================================
	struct ecsRotation: ecsIComponent {
		static const char *  s_name;
		void Init() { rotation = btQuaternion::getIdentity(); }

		btQuaternion	rotation = btQuaternion::getIdentity();
	};

	//================================
	struct ecsScaling : ecsIComponent {
		static const char *  s_name;
		void Init() { scale = btVector3::One(); }

		btVector3		scale	= btVector3::One();
	};

	//================================
	struct ecsMovement : ecsIComponent {
		static const char *  s_name;
		void Init() { speed = btVector3::Zero(); }

		btVector3		speed	= btVector3::Zero();
	};

	//================================
	struct ecsParticle : ecsIComponent {
		static const char * s_name;
		void Init() {
			color = Color::Red; 
			durationLeft = 1.f;
		}

		fan::Color	color			= Color::Red;
		float		durationLeft	= 1.f;
	};

	//================================
	struct ecsAABB : ecsIComponent {
		static const char * s_name;
		void Init() {	aabb.Clear(); }

		AABB aabb;
	};

	//================================
	struct ecsModel : ecsIComponent {
		static const char * s_name;
		void Init() { 
			mesh = nullptr; 
			renderID = -1;
		}

		Mesh * mesh			= nullptr;
		int renderID		= -1;
	};

	//================================
	struct ecsDirLight : ecsIComponent {
		static const char * s_name;
		void Init() {
			ambiant = Color::Black;
			diffuse = Color::White;
			specular = Color::White;
		}

		Color ambiant		= Color::Black;
		Color diffuse		= Color::White;
		Color specular		= Color::White;
	};

	//================================
	struct ecsPointLight : ecsIComponent {
		static const char * s_name;
		void Init() {
			ambiant = Color::White;
			diffuse = Color::White;
			specular = Color::White;
			attenuation[0] = 0.f; attenuation[1] = 0.f; attenuation[2] = 0.1f;
		}

		Color ambiant			= Color::White;
		Color diffuse			= Color::White;
		Color specular			= Color::White;
		float attenuation[3]	= {0.f,0.f,0.1f};
	};

	//================================
	struct ecsMaterial : ecsIComponent {
		static const char * s_name;
		void Init() {
			texture = nullptr;
			shininess = 1;
			color = Color::White;
		}

		Texture *	texture			= nullptr;
		uint32_t	shininess		= 1;
		Color		color			= Color::White;
	};

	//================================
	struct ecsPlanet : ecsIComponent {
		static const char * s_name;
		void Init() {
			speed = 1.f;
			radius = 1.f;
			phase = 0.f;
			parentEntity = ecsNullEntity;
		}

		float speed		= 1.f;
		float radius	= 1.f;
		float phase		= 0.f;
		ecsEntity parentEntity; // Updated in the component before the ecs call TODO remove this trash
	}; 

	//================================
	struct ecsRigidbody : ecsIComponent {
		static const char * s_name;
		char bufferRigidbody[sizeof( btRigidBody )]; // dummy btRigidBody memory to bypass btRigidBody constructor
		
		btRigidBody * Init( const btRigidBody::btRigidBodyConstructionInfo& constructionInfo ) {
			return new( bufferRigidbody ) btRigidBody( constructionInfo );
		}
		inline btRigidBody& Get() { return *reinterpret_cast<btRigidBody*>( bufferRigidbody ); }

	}; static_assert( sizeof(ecsRigidbody) == sizeof( btRigidBody ) );

	//================================
	struct ecsMotionState : ecsIComponent {
		static const char * s_name;
		char bufferMotionState[sizeof( btDefaultMotionState )]; // dummy btMotionState memory to bypass btDefaultMotionState constructor

		btDefaultMotionState * Init(	const btTransform &_startTrans = btTransform::getIdentity(),
										const btTransform &_centerOfMassOffset = btTransform::getIdentity() ) {
			return new( bufferMotionState ) btDefaultMotionState( _startTrans, _centerOfMassOffset );
		}
		inline btDefaultMotionState& Get() { return *reinterpret_cast<btDefaultMotionState*>( bufferMotionState ); }

	}; static_assert( sizeof( ecsMotionState ) == sizeof( btDefaultMotionState ) );

	//================================
	struct ecsSphereShape : ecsIComponent {
		static const char * s_name;
		char bufferSphereShape[sizeof( btSphereShape )]; // dummy btSphereShape memory to bypass btDefaultMotionState constructor

		btSphereShape * Init( const float _radius ) {
			return new( bufferSphereShape ) btSphereShape( _radius );
		}
		inline btSphereShape& Get() { return *reinterpret_cast<btSphereShape*>( bufferSphereShape ); }

	}; static_assert( sizeof( ecsSphereShape ) == sizeof( btSphereShape ) );

	//================================
	struct ecsBoxShape : ecsIComponent {
		static const char * s_name;
		char bufferBoxShape[sizeof( btBoxShape )]; // dummy btBoxShape memory to bypass btDefaultMotionState constructor

		btBoxShape * Init( const btVector3 _boxHalfExtents ) {
			return new( bufferBoxShape ) 	btBoxShape( _boxHalfExtents );
		}
		inline 	btBoxShape& Get() { return *reinterpret_cast<btBoxShape*>( bufferBoxShape ); }

	}; static_assert( sizeof( ecsBoxShape ) == sizeof( btBoxShape ) );
	
	//================================
	struct ecsFlags : ecsIComponent {
		static const char * s_name;
		void Init() { flags = 0; }

		uint32_t flags = 0;

		enum Flag {
			NONE = 1 << 0, NO_DELETE = 1 << 1, NOT_SAVED = 1 << 2, OUTDATED_TRANSFORM = 1 << 3, OUTDATED_MATERIAL = 1 << 4, OUTDATED_LIGHT = 1 << 5, NO_AABB_UPDATE = 1 << 6
		};
	};

	//================================
	struct ecsConvexHull : ecsIComponent {
		static const char * s_name;
		void Init() { convexHull.Clear(); }

		ConvexHull convexHull;
	};

	//================================
	//================================
	using ecsComponents = meta::TypeList<
		ecsTranform
		, ecsPosition
		, ecsRotation
		, ecsMovement
		, ecsParticle
		, ecsScaling
		, ecsAABB
		, ecsModel
		, ecsPointLight
		, ecsDirLight
		, ecsMaterial
		, ecsPlanet
		, ecsRigidbody
		, ecsMotionState
		, ecsSphereShape
		, ecsBoxShape
		, ecsFlags
		, ecsConvexHull
	>;
	 
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< ecsIComponent, _type >::value; };
}