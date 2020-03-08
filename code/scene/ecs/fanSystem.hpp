#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	//================================================================================================================================
	// system
	//================================================================================================================================
#define DECLARE_SYSTEM()						\
	private:									\
	friend class EntityWorld;					\
	static const char* s_typeName;				\

#define REGISTER_SYSTEM( _systemType, _name)							\
	const char* _systemType::s_typeName = _name;						\

	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entites
	//
	// System must call the (DECLARE/REGISTER)_SYSTEM macro and implement Clear() method that set its signature.
	// It also must implement a static Run(..) method that runs its logic.
	// It also must be registered in the EntityWorld constructor to be assigned a unique signature
	//==============================================================================================================================================================
	struct System
	{};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UpdateAABBFromRigidbodySystem : System
	{
		DECLARE_SYSTEM()
	public:

		static Signature GetSignature( const EntityWorld& _world );
		static void Run( EntityWorld& _world, const std::vector<Entity*>& _entities, const float _delta );
	};
}